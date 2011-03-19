#!/usr/local/bin/lua

dlopen ("libkinoparserlua.so")
dlopen ("libkinoahdlua.so")

a = AHDRuntimeNew (0, nil)

--globalvar = "Hallo"

t = read (".*")

--d = AHDRuntimeLoad (a, "http://localhost:9999/test.xml")

function parse_xml_text (t)
   d = DocumentNew ()
   p = ParserNew ()
   r = ElementNew ()
   
   ParserSetStartElement (p, r)
   ParserSetDocument (p, d)
   NodeAppendChild (d, r)
   ParserSetURI (p, "")
   AHDRuntimeDocumentInit (a, d)
   ParserProcessData (p, "text/xml", t)

   return d
end

d1 = parse_xml_text (t)
d2 = parse_xml_text ([[<parameter>
<func name='onmatch' type='text/lua'>
print (ElementGetTagName (ahd_caller) .. ": " .. ElementGetContents (ahd_caller))
</func>
<description/>
<type/>
</parameter>]])

source = DocumentGetElementsByTagName (d1, "interface")
pattern = DocumentGetElementsByTagName (d2, "parameter")

l = AHDRuntimeMatch (a,
   NodeListGetItem (source, 0), NodeListGetItem (pattern, 0))

