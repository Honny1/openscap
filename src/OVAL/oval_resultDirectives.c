/**
 * @file oval_resultDirectives.c
 * \brief Open Vulnerability and Assessment Language
 *
 * See more details at http://oval.mitre.org/
 */

/*
 * Copyright 2008 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *      "David Niemoller" <David.Niemoller@g2-inc.com>
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <common/util.h>
#include "oval_results_impl.h"
#include "oval_collection_impl.h"


struct _oval_result_directive{
	bool                               reported;
	oval_result_directive_content_t content;
};

#define NUMBER_OF_RESULTS 7

typedef struct oval_result_directives {
	struct _oval_result_directive directive[NUMBER_OF_RESULTS];
} oval_result_directives_t;

struct oval_result_directives *oval_result_directives_new()
{
	oval_result_directives_t *directives = (oval_result_directives_t *)
		malloc(sizeof(oval_result_directives_t));
	int i;for(i=0;i<NUMBER_OF_RESULTS;i++){
		directives->directive[i].reported = false;
		directives->directive[i].content  = OVAL_DIRECTIVE_CONTENT_UNKNOWN;
	}
	return directives;
}
void oval_result_directives_free(struct oval_result_directives *directives)
{
	free(directives);
}

bool oval_result_directive_get_reported
	(struct oval_result_directives *directives, oval_result_t type)
{
	return directives->directive[type].reported;
}
oval_result_directive_content_t oval_result_directive_get_content
	(struct oval_result_directives *directives, oval_result_t type)
{
	return directives->directive[type].content;
}
void oval_result_directive_set_reported
	(struct oval_result_directives *directives, oval_result_t type, bool reported)
{
	directives->directive[type].reported = reported;
}
void oval_result_directive_set_content
	(struct oval_result_directives *directives, oval_result_t type, oval_result_directive_content_t content)
{
	directives->directive[type].content = content;
}

//typedef int (*oval_xml_tag_parser) (xmlTextReaderPtr, struct oval_parser_context *, void *);
static int _oval_result_directives_parse_tag
	(xmlTextReaderPtr reader, struct oval_parser_context *context, void *client)
{
	struct oval_result_directives *directives = (struct oval_result_directives *)client;
	oval_result_directive_content_t type = OVAL_DIRECTIVE_CONTENT_UNKNOWN;
	char *tag_names[NUMBER_OF_RESULTS] =
	{
		NULL
		,"definition_true"
		,"definition_false"
		,"definition_unknown"
		,"definition_error"
		,"definition_not_evaluated"
		,"definition_not_applicable"
	};
	int i, retcode = 1;
	xmlChar *name = xmlTextReaderLocalName(reader);
	for(i=1;i<NUMBER_OF_RESULTS && type==OVAL_DIRECTIVE_CONTENT_UNKNOWN;i++){
		if(strcmp(tag_names[i],name)==0){
			type = i;
		}
	}
	if(type){
		{//reported
			char* boolstr = xmlTextReaderGetAttribute(reader, "reported");
			bool reported = strcmp(boolstr,"1")==0 || strcmp(boolstr,"true")==0;
			free(boolstr);
			oval_result_directive_set_reported(directives, type, reported);
		}
		{//content
			xmlChar *contentstr =  xmlTextReaderGetAttribute(reader, "content");
			oval_result_directive_content_t content = OVAL_DIRECTIVE_CONTENT_UNKNOWN;
			if(contentstr){
				char *content_names[3] = {NULL,"thin", "full"};
				for(i=1;i<3 && content==OVAL_DIRECTIVE_CONTENT_UNKNOWN;i++){
					if(strcmp(content_names[i],contentstr)==0){
						content = i;
					}
				}
				if(content){
					oval_result_directive_set_content(directives, type, content);
				}else{
					char message[200];
					sprintf(message, "_oval_result_directives_parse_tag: cannot resolve @content=\"%s\"",contentstr);
					oval_parser_log_warn(context, message);
					retcode = 0;
				}
				free(contentstr);
			}else{
				content = OVAL_DIRECTIVE_CONTENT_FULL;
			}
		}
	}else{
		char message[200];
		sprintf(message, "_oval_result_directives_parse_tag: cannot resolve <%s>",name);
		oval_parser_log_warn(context, message);
		retcode = 0;
	}
	free(name);
	return retcode;
}

int oval_result_directives_parse_tag
	(xmlTextReaderPtr reader, struct oval_parser_context *context, struct oval_result_directives *directives)
{
	return oval_parser_parse_tag(reader, context, &_oval_result_directives_parse_tag, directives);
}

static const struct oscap_string_map _OVAL_DIRECTIVE_MAP[] = {
		{ OVAL_RESULT_TRUE          , "definition_true"          },
		{ OVAL_RESULT_FALSE         , "definition_false"         },
		{ OVAL_RESULT_UNKNOWN       , "definition_unknown"       },
		{ OVAL_RESULT_ERROR         , "definition_error"         },
		{ OVAL_RESULT_NOT_EVALUATED , "definition_not_evaluated" },
		{ OVAL_RESULT_NOT_APPLICABLE, "definition_not_applicable"},
		{ OVAL_RESULT_INVALID       , NULL }
};

int oval_result_directives_to_dom
	(struct oval_result_directives *directives, xmlDoc *doc, xmlNode *parent)
{
	int retcode = 1;
	xmlNs *ns_results  = xmlSearchNsByHref(doc, parent, OVAL_RESULTS_NAMESPACE);
	xmlNode *directives_node = xmlNewChild
		(parent, ns_results, BAD_CAST "directives", NULL);

	const struct oscap_string_map *map;
	for(map = _OVAL_DIRECTIVE_MAP;map->string; map++)
	{
		oval_result_t directive = (oval_result_t)
			map->value;
		bool reported = oval_result_directive_get_reported
			(directives, directive);
		oval_result_directive_content_t content = oval_result_directive_get_content
			(directives, directive);
		xmlNode *directive_node = xmlNewChild
			(directives_node, ns_results, (map->string),NULL);
		char *val_reported = (reported)?"true":"false";
		char *val_content  = (content==OVAL_DIRECTIVE_CONTENT_FULL)
			?"full":"thin";
		xmlNewProp(directive_node, BAD_CAST "reported", BAD_CAST val_reported);
		xmlNewProp(directive_node, BAD_CAST "content", BAD_CAST val_content);
	}
	return retcode;
}

