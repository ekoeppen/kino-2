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

proc handleNode {node} {
  while {$node != "NULL"} {
    switch [NodeGetNodeType $node] {
      1 {
	puts "<[ElementGetTagName $node]>"
	if {[NodeHasChildNodes $node]} {
	  handleNode [NodeGetFirstChild $node]
	}
	puts "</[ElementGetTagName $node]>"
      }
      3 {
	puts -nonewline [TextGetData $node]
      }
      9 {
	if {[NodeHasChildNodes $node]} {
	  handleNode [NodeGetFirstChild $node]
	}
      }
    }
    set node [NodeGetNextSibling $node]
  }
}

set d [ParseXMLText "<p>Hello, world!</p>"]

handleNode $d