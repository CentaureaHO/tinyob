#!/bin/bash

flex --outfile lexer.cpp --header-file=lexer.h lexer.l
`which bison` -d --output parser.cpp parser.y -Wcounterexamples
