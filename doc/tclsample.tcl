#!/usr/bin/tclsh

load libkinoparsertcl.so kinoparser

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

set d [ParseXMLText "<p>Hello, world!</p>"]

puts [NodeToText $d]