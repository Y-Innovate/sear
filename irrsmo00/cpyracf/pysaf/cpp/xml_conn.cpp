#include <cstdlib>

#include "../h/xml_conn.h"
#include "../h/saf_xml.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

char * injson_to_inxml(char * injson, char * userid_buffer, bool debug){
    XmlGen * xml = new XmlGen();
    return xml->build_xml_string(injson, userid_buffer, debug);
}

char * outxml_to_outjson(char * outxml, bool debug){
    XmlParse * xml = new XmlParse();
    return xml->build_json_string(outxml, debug);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */