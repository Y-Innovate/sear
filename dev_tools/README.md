## Description

This is a folder where we store tools that were useful for the development of RACFu but do not hold any meaninful code. The files and their natures are explained below in sections based on the purpose of the file.

## Unit Test Generation/Conversion

This/these files were helpful in either generating unit tests or converting files involved in unit tests. RACFu uses a lot of binary files that are ebcdic-encoded xml data as an example, so this was beneficial for making quick adjustments to the files used in these tests.

### convert_dirs.py

This is a python script that contains a number of functions designed to encode/decode files to and from one ebcdic codepage to another (or ascii). These are used in the script to take a folder of xml files in ascii encoding (e.g. `request_xml`) and convert these to binary files in a similarly named folder in IBM-1047 ebcdic (e.g. `request_bin`).

## Key Mapping Generation/Compilation

This/these files were a part of how key mappings for both RACFu and pyRACF were generated. These scripts were helpful tools, but they alone did not generate the final mappings used in RACFu or pyRACF. These tools are, as a result, somewhat incomplete, but including them still felt like it would help if someone were to try and replicate this work.

### load_seg_data.py

Attempts to convert an xlsx Microsoft Excel spreadsheet with the data for the "x administration" callable service tables like in [IBM Documentation](https://www.ibm.com/docs/en/zos/3.1.0?topic=tables-user-administration). Manually copying this data to an "x_admin.xlsx" file allows you to use this script to somewhat convert that to a json file with most of the data needed to generate meaningful key mappings (but not everything). I know for a fact that many booleans were missed, repeat groups weren't properly accounted for and dataset and setropts administration tables were not copied appropriately.

### map_fields.py

Takes the json file generated with the `load_seg_data.py` file and offers functions that attmept to format this into the c++ key_map structure files that RACFu uses. At one point this also generated the key mappings for pyRACF, but I believe those features were pruned to establish the RACFu ones. Takes in the `x_admin.json` files generated in the previous step and generates appropriate c header files. Conceivably this file works well, and just collecting the appropriate json data would enable this to perform its function perfectly.
