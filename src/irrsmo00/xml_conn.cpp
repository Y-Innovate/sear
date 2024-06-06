#include <cstdlib>

#include "xml_conn.h"
#include "saf_xml.h"

// Connects the "XML library" to the C layer with these extern C functions

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

char * injson_to_inxml(char * injson, char * userid_buffer, int * irrsmo00_options, unsigned int * result_buffer_size, bool * debug){
    XmlGen * xml = new XmlGen();
    return xml->build_xml_string(injson, userid_buffer, irrsmo00_options, result_buffer_size, debug);
}

char * outxml_to_outjson(char * outxml, int saf_rc, int racf_rc, int racf_rsn, bool debug){
    XmlParse * xml = new XmlParse();
    return xml->build_json_string(outxml, saf_rc, racf_rc, racf_rsn, debug);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */