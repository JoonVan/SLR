﻿#ifndef __SLRSceneGraph_SceneParsingDriver__
#define __SLRSceneGraph_SceneParsingDriver__

#include <libSLR/defines.h>
#include "../declarations.h"
#include "SceneParser.tab.hh"

#define YY_DECL \
SLRSceneGraph::SceneParser::symbol_type yylex(SLRSceneGraph::SceneParsingDriver &driver)
YY_DECL;

namespace SLRSceneGraph {    
    struct SceneParsingDriver {        
        std::string file;
        bool traceScanning;
        bool traceParsing;
        location currentLocation;
        
        StatementsRef statements;

        SceneParsingDriver() : 
        traceScanning(false), traceParsing(false) {

        }
        ~SceneParsingDriver() { }

        void beginScan();
        void endScan();

        StatementsRef parse(const std::string &f) {
            file = f;
            beginScan();
            SceneParser parser(*this);
            parser.set_debug_level(traceParsing);
            int res = parser.parse();
            endScan();

            return res == 0 ? statements : nullptr;
        }

        void error(const location &loc, const std::string &msg) {
            std::cerr << loc << ": " << msg << std::endl;
        }
        void error(const std::string &msg) {
            std::cerr << msg << std::endl;
        }
    };
}

#endif /* __SLRSceneGraph_SceneParsingDriver__ */
