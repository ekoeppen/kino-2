#!/usr/bin/tclsh

load libkinoparsertcl.so kinoparser
load linkinoahdtcl.so kinoahd

proc ParseXMLText {text} {
  set document [DocumentNew]
  set parser [ParserNew]
  set root [ElementNew]

  ParserSetDocument $parser $document
  NodeAppendChild $document $root
  ParserSetStartElement $parser $root

  ElementSetTagName $root "KINO2:ROOT"

  ParserSetURI $parser ""
  ParserProcessData $parser "text/xml" $text
  ParserDelete $parser

  return $document
}

set d [ParseXMLText "<a><b><c><d/><e/></c><f/><g/></b><h/></a>"]

set l [DocumentGetElementsByTagName $d *]
for {set i 0} {[expr $i < [NodeListGetLength $l]]} {incr i} {
  puts [ElementGetTagName [NodeListGetItem $l $i]]
}