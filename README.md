# OpenSCAP

[![Build status](https://ci.appveyor.com/api/projects/status/3o5fnld1m98bo0so/branch/master?svg=true)](https://ci.appveyor.com/project/OpenSCAP/openscap/branch/master)

Open Source Security Compliance Solution

## About

The oscap program is a command line tool that allows users to load, scan,
validate, edit, and export SCAP documents.

 * Homepage of the project: [www.open-scap.org](http://www.open-scap.org)
 * Manual: [Oscap User Manual](https://github.com/OpenSCAP/openscap/blob/master/docs/manual/manual.adoc)
 * Compilation, testing and debugging: [OpenSCAP Developer Manual](https://github.com/OpenSCAP/openscap/blob/master/docs/developer/developer.adoc)
 * For new contributors: [How to contribute](https://github.com/OpenSCAP/openscap/blob/master/docs/contribute/contribute.adoc)

## Use cases

### SCAP Content Validation

 * The following example shows how to validate a given source data stream; all components within the data stream are validated (XCCDF, OVAL, OCIL, CPE, and possibly other components):
```
oscap ds sds-validate scap-ds.xml
```


### Scanning

 * To evaluate all definitions within the given OVAL Definition file, run the following command:
```
oscap oval eval --results oval-results.xml scap-oval.xml
```
*where scap-oval.xml is the OVAL Definition file and oval-results.xml is the OVAL Result file.*

 * To evaluate all definitions from the OVAL component that are part of a particular data stream within a SCAP data stream collection, run the following command:
```
oscap oval eval --datastream-id ds.xml --oval-id xccdf.xml --results oval-results.xml scap-ds.xml
```
*where ds.xml is the given data stream, xccdf.xml is an XCCDF file specifying the OVAL component, oval-results.xml is the OVAL Result file, and scap-ds.xml is a file representing the SCAP data stream collection.*

 * To evaluate a specific profile in an XCCDF file run this command:
```
oscap xccdf eval --profile Desktop --results xccdf-results.xml --cpe cpe-dictionary.xml scap-xccdf.xml
```
*where scap-xccdf.xml is the XCCDF document, Desktop is the selected profile from the XCCDF document, xccdf-results.xml is a file storing the scan results, and cpe-dictionary.xml is the CPE dictionary.*

 * To evaluate a specific XCCDF benchmark that is part of a data stream within a SCAP data stream collection run the following command:
```
oscap xccdf eval --datastream-id ds.xml --xccdf-id xccdf.xml --results xccdf-results.xml scap-ds.xml
```
*where scap-ds.xml is a file representing the SCAP data stream collection, ds.xml is the particular data stream, xccdf.xml is ID of the component-ref pointing to the desired XCCDF document, and xccdf-results.xml is a file containing the scan results.*


### Document generation

 * without XCCDF rules
```
oscap xccdf generate guide XCCDF-FILE > XCCDF-GUIDE-FILE
```

 * with XCCDF rules
```
oscap xccdf generate guide --profile PROFILE XCCDF-FILE > XCCDF-GUIDE-FILE
```

 *  generate report from scanning
```
oscap xccdf generate report XCCDF-RESULT-FILE > XCCDF-REPORT-FILE
```
