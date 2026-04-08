#include "lexer/keywords.h"
#include <unordered_map>
using namespace std;

static const std::pmr::unordered_map<string_view, Keyword> keywords = {
    {"_",           Keyword::WC         },
    {"await",       Keyword::Await      },
    {"break",       Keyword::Break      },
    {"class",       Keyword::Class      },
    {"const",       Keyword::Const      },
    {"continue",    Keyword::Continue   },
    {"else",        Keyword::Else       },
    {"enum",        Keyword::Enum       },
    {"extend",      Keyword::Extend     },
    {"extern",      Keyword::Extern     },
    {"false",       Keyword::False      },
    {"final",       Keyword::Final      },
    {"for",         Keyword::For        },
    {"if",          Keyword::If         },
    {"in",          Keyword::In         },
    {"interface",   Keyword::Interface  },
    {"internal",    Keyword::Internal   },
    {"let",         Keyword::Let        },
    {"match",       Keyword::Match      },
    {"module",      Keyword::Module     },
    {"move",        Keyword::Move       },
    {"null",        Keyword::Null       },
    {"private",     Keyword::Private    },
    {"protected",   Keyword::Protected  },
    {"public",      Keyword::Public     },
    {"return",      Keyword::Return     },
    {"spawn",       Keyword::Spawn      },
    {"static",      Keyword::Static     },
    {"super",       Keyword::Super      },
    {"this",        Keyword::This       },
    {"true",        Keyword::True       },
    {"try",         Keyword::Try        },
    {"type",        Keyword::Type       },
    {"typeof",      Keyword::Typeof     },
    {"var",         Keyword::Var        },
    {"void",        Keyword::Void       },
    {"where",       Keyword::Where      },
    {"while",       Keyword::While      },
    {"yield",       Keyword::Yield      },
    // Weak
    {"get",         Keyword::Get        },
    {"include",     Keyword::Include    },
    {"set",         Keyword::Set        }
};

std::optional<Keyword> getKeyword(string_view s) {
    auto it = keywords.find(s);
    if (it == keywords.end()) return nullopt;
    return it->second;
};