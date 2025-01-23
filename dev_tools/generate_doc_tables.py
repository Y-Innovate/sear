import os
import sys
import re

def convert_key_map_hpp_to_doc(input_filepath, output_filepath):
    alter_only_admin_types = ["Racf-Options", "Permission", "Group-Connection"]
    if input_filepath.split('.')[1] != "hpp" or output_filepath.split('.')[1] != "md":
        print("whoops, wrong file!")
    admin_type = output_filepath.split('.')[0].split('/')[1].replace("_","-").title()
    operation_types = "add and alter operations,"
    if admin_type in alter_only_admin_types:
        operation_types = "alter operations"
    doc_file_data = f"---\nlayout: default\nparent: Traits\n---\n\n# {admin_type} Traits\n\n" + \
    f"The following tables describes the {admin_type.lower()} segments and traits that are" + \
    f" supported for {operation_types} and returned by extract operations.\n" + \
    "{: .fs-6 .fw-300 }\n\n&nbsp;\n\n{: .note }\n" + \
    "> _More information about **RACF Keys** can be found [here]" + \
    "(https://www.ibm.com/docs/en/zos/3.1.0?topic=tables-user-administration)._" + \
    "\n\n&nbsp;\n\n{: .note }\n" + \
    "> _See [Data Types](../data_types) for more information about **Data Types**._" + \
    "\n\n&nbsp;\n\n{: .note }\n" + \
    "> _See [Operators](../operators) for more information about **Operator** usage._\n"

    f = open(input_filepath, "r")
    header_file_data = f.read()
    f.close()

    segement_trait_information = header_file_data.split('segment_key_mapping_t')[0]

    segment_mapping = f"{admin_type.replace("-","_").upper()}_([A-Z]*)(?<!SEGMENT)_(?:SEGMENT|KEY)_MAP"

    segments = re.findall(segment_mapping, segement_trait_information)
    
    for segment in segments:
        if segment == "BASE":
            segment = segment.title()
        else:
            segment = segment.upper()
        print(segment)
        doc_file_data = doc_file_data + f"\n## {segment} Segment\n\n" + \
        "| **Trait** | **RACF Key** | **Data Types** | **Operators Allowed** | **Supported Operations** |\n"
        trait_mapping = f"\"({segment.lower()}:[a-z_]*)\"," + \
        ".*\"([a-z]*)\",\n.*TRAIT_TYPE_([A-Z]*),.*\{(true|false), (true|false), (true|false), (true|false)\}"
        traits = re.findall(trait_mapping, segement_trait_information)
        for trait in traits:
            print(trait)
            operators_allowed = []
            if trait[3] == "true":
                operators_allowed.append('`"set"`')
                supported_operations = ['`"add"`', '`"alter"`', '`"extract"`']
            if trait[4] == "true":
                operators_allowed.append('`"add"`')
                supported_operations = ['`"add"`', '`"alter"`', '`"extract"`']
            if trait[5] == "true":
                operators_allowed.append('`"remove"`')
                supported_operations = ['`"add"`', '`"alter"`', '`"extract"`']
            if trait[6] == "true":
                operators_allowed.append('`"delete"`')
                supported_operations = ['`"add"`', '`"alter"`', '`"extract"`']
            if operators_allowed == []:
                operators_allowed = ["N/A"]
                supported_operations = ['`"extract"`']
            doc_file_data = doc_file_data + \
            f"| `\"{trait[0]}\"` | `{trait[1]}` | `{trait[2].lower()}` | {"<br>".join(operators_allowed)} | {"<br>".join(supported_operations)} |\n"
    
    #TODO Check JSON Data for supported Operations
    #TODO Fix JSON Data for Dataset and Setropts Admins
    #TODO remove "undocumented" traits/segments
    
    f = open(output_filepath, "w")
    f.write(doc_file_data)
    f.close()
    return 0

def convert_directory(directory_path):
    ignore_list = ["key_map.cpp", "key_map.hpp", "key_map_structs.hpp"]
    for file_name in os.listdir(directory_path):
        if file_name in ignore_list:
            continue
        output_name = file_name.split("key_map_")[1].split('.')[0]+".md"
        print(f"Converting {file_name} to {output_name} for documentation purposes...")
        convert_key_map_hpp_to_doc(directory_path+"/"+file_name, "md/"+output_name)

directory_path = sys.argv[1]
convert_directory(directory_path)