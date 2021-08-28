/**\file
 * Basic Concepts:
 *
 * - arg_parser - object for storing argument descriptions
 *
 * - arg_desc - description of one argument
 *
 *
 * Usage:
 *
 * 1. Create parser by calling arg_parser_make
 *
 * 2. Define flags for parser by ARG_PARSER_ADD_* macroses
 *
 * 3. Call #ARG_PARSER_PARSE for parsing command line argument. Note, check
 * return value for make sure that parsing was successfull, or check err string
 *
 * 4. Use ARG_PARSER_GET_* macroses for getting parsed values
 *
 * 5. Call arg_parser_dispose for destroy parser before exit
 */

#pragma once


#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define ARG_MAX_VALUE_LEN   64
#define ARG_MAX_ERROR_LEN   1024
#define ARG_MAX_FMT_ARG_LEN 128


#ifdef __cplusplus
extern "C" {
#endif

enum ArgType {
  ArgBool,
  ArgString,
  ArgInt,
  ArgLong,
  ArgLongLong,
  ArgDouble,
};
enum ArgFlags {
  ArgNone     = 0,
  ArgDefault  = 0b001,
  ArgRequired = 0b010,
  ArgFound    = 0b100,
};


union ArgUnion {
  bool        val_bool;
  int         val_int;
  long        val_long;
  long long   val_ll;
  double      val_double;
  const char *val_str;
};


typedef struct _arg_desc {
  char *         name; // argument name (without first --)
  char *         desc; // description
  union ArgUnion dval; // default value
  enum ArgType   type; // arg type
  int            flgs; // arg flags
  char           shrt; // short name
} arg_desc;

typedef struct _arg_rval {
  char *         name;
  union ArgUnion rval;
  enum ArgType   type;
} arg_rval;


typedef struct _arg_parser {
  char *    mdesc; // main description
  arg_desc *alist; // list of described args
  arg_rval *rlist; // list with return values
  uint      asize; // count of described args
  uint      rsize; // count of return values
} arg_parser;


enum ArgType typename2argtype(const char *type_name);
char *       val_to_string(union ArgUnion val, enum ArgType type);
char *       str_to_arg_name(const char *name);
int          str_arg_cmp(const char *lhs, const char *rhs);
int          arg_name_cmp(const char *arg_name,
                          char        short_name,
                          const char *val_for_comp);

union ArgUnion arg_union_make_from_str(const char *val);
union ArgUnion arg_union_make_from_bool(bool val);
union ArgUnion arg_union_make_from_int(int val);
union ArgUnion arg_union_make_from_long(long val);
union ArgUnion arg_union_make_from_ll(long long val);
union ArgUnion arg_union_make_from_double(double val);

void *void_ptr_cast_from_str(const char **val);
void *void_ptr_cast_from_bool(bool *val);
void *void_ptr_cast_from_int(int *val);
void *void_ptr_cast_from_long(long *val);
void *void_ptr_cast_from_ll(long long *val);
void *void_ptr_cast_from_double(double *val);


/**\brief create instance of arg_parser
 * \param main_desc description of your program, could be a NULL
 */
arg_parser *arg_parser_make(const char *main_desc);

/**\brief destroy instance of arg_parser
 */
void arg_parser_dispose(arg_parser *parser);

/**\return description of flags
 * \note you should free returned string after usage
 */
char *arg_parser_usage(arg_parser *parser);

void arg_parser_add_arg(arg_parser *   parser,
                        const char *   name,
                        char           short_name,
                        const char *   desc,
                        enum ArgType   type,
                        int            flags,
                        union ArgUnion default_val);

/**\return count of arg values with given name
 */
int arg_parser_count(arg_parser *parser, const char *name);

/**\return capacity of returned values
 * \param count desired count of values for return associated with given flag
 * \note val should be a pointer with allocated space not less then count
 */
int arg_parser_get_args(const arg_parser *parser,
                        const char *      name,
                        enum ArgType      type,
                        void *            val,
                        int               count);

/**\brief parse args by parser
 * \return 0 if parsing was successfull, otherwise return not 0 value
 * \param ignore_not_defined_flags if false, then parsing fail if unknown flag
 * found
 * \param remove_flags_from_argv removes all defined flags from argv except
 * program name, positional arguments and not defined flags
 * \param err if non NULL, then parser store string with error if parsing
 * failed. NOTE: you should call free for the string after using
 */
int arg_parser_parse(arg_parser *parser,
                     int *       argc,
                     char **     argv[],
                     bool        ignore_not_defined_flags,
                     bool        remove_defined_flags_from_argv,
                     char **     err);


/**\brief parsing args
 * \return 0 if parsing was successfull, otherwise return non-zero value
 * \param err pointer to c string or NULL
 * \param ignore_not_defined_flags set to false if you want to parser failed if
 * some unexpected flag founded
 * \param remove_defined_flags_from_argv set to true if you want leave in argv
 * only program name, positional args and not defined flags
 */
#define ARG_PARSER_PARSE(parser,                         \
                         argc,                           \
                         argv,                           \
                         ignore_not_defined_flags,       \
                         remove_defined_flags_from_argv, \
                         err)                            \
  arg_parser_parse(parser,                               \
                   &argc,                                \
                   &argv,                                \
                   ignore_not_defined_flags,             \
                   remove_defined_flags_from_argv,       \
                   err)


/**\brief add argument description to parser
 * \param parser arg_parser object
 * \param key name of argument
 * \param short_name short name (one symbol) of the argument, 0 if not provided
 * \param type type of argument
 * \param default_val default value for argument
 * \param flags argument flags @see ArgFlags
 * \return 0 if case of success, otherwise non zero value
 */
#define ARG_PARSER_ADD_ARG(parser,            \
                           key,               \
                           short_name,        \
                           description,       \
                           type,              \
                           default_val,       \
                           flags)             \
  arg_parser_add_arg(parser,                  \
                     key,                     \
                     short_name,              \
                     description,             \
                     typename2argtype(#type), \
                     flags,                   \
                     arg_union_make_from_##type(default_val))


#define ARG_PARSER_ADD_STR(parser, key, short_name, description, is_required) \
  ARG_PARSER_ADD_ARG(parser,                                                  \
                     key,                                                     \
                     short_name,                                              \
                     description,                                             \
                     str,                                                     \
                     NULL,                                                    \
                     (is_required) ? ArgRequired : 0)

#define ARG_PARSER_ADD_INT(parser, key, short_name, description, is_required) \
  ARG_PARSER_ADD_ARG(parser,                                                  \
                     key,                                                     \
                     short_name,                                              \
                     description,                                             \
                     int,                                                     \
                     0,                                                       \
                     (is_required) ? ArgRequired : 0)

#define ARG_PARSER_ADD_LONG(parser, key, short_name, description, is_required) \
  ARG_PARSER_ADD_ARG(parser,                                                   \
                     key,                                                      \
                     short_name,                                               \
                     description,                                              \
                     long,                                                     \
                     0,                                                        \
                     (is_required) ? ArgRequired : 0)

#define ARG_PARSER_ADD_LL(parser, key, short_name, description, is_required) \
  ARG_PARSER_ADD_ARG(parser,                                                 \
                     key,                                                    \
                     short_name,                                             \
                     description,                                            \
                     ll,                                                     \
                     0,                                                      \
                     (is_required) ? ArgRequired : 0)

#define ARG_PARSER_ADD_DOUBLE(parser,      \
                              key,         \
                              short_name,  \
                              description, \
                              is_required) \
  ARG_PARSER_ADD_ARG(parser,               \
                     key,                  \
                     short_name,           \
                     description,          \
                     double,               \
                     0,                    \
                     (is_required) ? ArgRequired : 0)

#define ARG_PARSER_ADD_BOOL(parser, key, short_name, description, is_required) \
  ARG_PARSER_ADD_ARG(parser,                                                   \
                     key,                                                      \
                     short_name,                                               \
                     description,                                              \
                     bool,                                                     \
                     NULL,                                                     \
                     (is_required) ? ArgRequired : 0)


#define ARG_PARSER_ADD_STRD(parser, key, short_name, description, default_val) \
  ARG_PARSER_ADD_ARG(parser,                                                   \
                     key,                                                      \
                     short_name,                                               \
                     description,                                              \
                     str,                                                      \
                     default_val,                                              \
                     ArgDefault)

#define ARG_PARSER_ADD_INTD(parser, key, short_name, description, default_val) \
  ARG_PARSER_ADD_ARG(parser,                                                   \
                     key,                                                      \
                     short_name,                                               \
                     description,                                              \
                     int,                                                      \
                     default_val,                                              \
                     ArgDefault)

#define ARG_PARSER_ADD_LONGD(parser,      \
                             key,         \
                             short_name,  \
                             description, \
                             default_val) \
  ARG_PARSER_ADD_ARG(parser,              \
                     key,                 \
                     short_name,          \
                     description,         \
                     long,                \
                     default_val,         \
                     ArgDefault)

#define ARG_PARSER_ADD_LLD(parser, key, short_name, description, default_val) \
  ARG_PARSER_ADD_ARG(parser,                                                  \
                     key,                                                     \
                     short_name,                                              \
                     description,                                             \
                     ll,                                                      \
                     default_val,                                             \
                     ArgDefault)

#define ARG_PARSER_ADD_DOUBLED(parser,      \
                               key,         \
                               short_name,  \
                               description, \
                               default_val) \
  ARG_PARSER_ADD_ARG(parser,                \
                     key,                   \
                     short_name,            \
                     description,           \
                     double,                \
                     default_val,           \
                     ArgDefault)

#define ARG_PARSER_ADD_BOOLD(parser,      \
                             key,         \
                             short_name,  \
                             description, \
                             default_val) \
  ARG_PARSER_ADD_ARG(parser,              \
                     key,                 \
                     short_name,          \
                     description,         \
                     bool,                \
                     default_val,         \
                     ArgDefault)


/**\brief return flag value
 * \param key complete name of flag
 * \param variable for return
 * \return 1 in case of success, otherwise return zero or less
 */
#define ARG_PARSER_GET_ARG(parser, key, val, type)     \
  arg_parser_get_args(parser,                          \
                      key,                             \
                      typename2argtype(#type),         \
                      void_ptr_cast_from_##type(&val), \
                      1)

#define ARG_PARSER_GET_STR(parser, key, val) \
  ARG_PARSER_GET_ARG(parser, key, val, str)

#define ARG_PARSER_GET_BOOL(parser, key, val) \
  ARG_PARSER_GET_ARG(parser, key, val, bool)

#define ARG_PARSER_GET_INT(parser, key, val) \
  ARG_PARSER_GET_ARG(parser, key, val, int)

#define ARG_PARSER_GET_LONG(parser, key, val) \
  ARG_PARSER_GET_ARG(parser, key, val, long)

#define ARG_PARSER_GET_LL(parser, key, val) \
  ARG_PARSER_GET_ARG(parser, key, val, ll)

#define ARG_PARSER_GET_DOUBLE(parser, key, val) \
  ARG_PARSER_GET_ARG(parser, key, val, double)


inline enum ArgType typename2argtype(const char *type_name) {
  if (strcmp(type_name, "str") == 0) {
    return ArgString;
  } else if (strcmp(type_name, "int") == 0) {
    return ArgInt;
  } else if (strcmp(type_name, "long") == 0) {
    return ArgLong;
  } else if (strcmp(type_name, "ll") == 0) {
    return ArgLongLong;
  } else if (strcmp(type_name, "double") == 0) {
    return ArgDouble;
  } else if (strcmp(type_name, "bool") == 0) {
    return ArgBool;
  }

  assert(0 &&
         "unknown typename, supported: str, int, long, ll, double and bool");
}

inline char *val_to_string(union ArgUnion val, enum ArgType type) {
  char *retval = (char *)malloc(ARG_MAX_VALUE_LEN);
  int   count  = 0;
  switch (type) {
  case ArgString:
    count = snprintf(retval, ARG_MAX_VALUE_LEN, "%s", val.val_str);
    break;
  case ArgInt:
    count = snprintf(retval, ARG_MAX_VALUE_LEN, "%i", val.val_int);
    break;
  case ArgLong:
    count = snprintf(retval, ARG_MAX_VALUE_LEN, "%li", val.val_long);
    break;
  case ArgLongLong:
    count = snprintf(retval, ARG_MAX_VALUE_LEN, "%lli", val.val_ll);
    break;
  case ArgBool:
    count = snprintf(retval,
                     ARG_MAX_VALUE_LEN,
                     "%s",
                     val.val_bool ? "true" : "false");
    break;
  case ArgDouble:
    count = snprintf(retval, ARG_MAX_VALUE_LEN, "%f", val.val_double);
    break;
  }
  if (count >= ARG_MAX_VALUE_LEN) {
    retval[ARG_MAX_VALUE_LEN - 1] = '\0';
  }
  return retval;
}

inline char *str_to_arg_name(const char *name) {
  uint  len    = strlen(name);
  char *retval = (char *)malloc(len + 1);
  strcpy(retval, name);
  for (uint i = 0; i < len; ++i) {
    if ('_' == retval[i]) {
      retval[i] = '-';
      continue;
    }
    retval[i] = tolower(retval[i]);
  }
  return retval;
}

inline int str_arg_cmp(const char *lhs, const char *rhs) {
  for (uint i = 0; i < strlen(lhs) && i < strlen(rhs); ++i) {
    char lhs_c = tolower(lhs[i]);
    char rhs_c = tolower(rhs[i]);
    if (lhs_c == '_') {
      lhs_c = '-';
    }
    if (rhs_c == '_') {
      rhs_c = '-';
    }

    if (lhs_c != rhs_c) {
      return i + 1;
    }
  }

  return 0;
}

inline int
arg_name_cmp(const char *arg_name, char short_name, const char *val_for_comp) {
  uint arg_len = strlen(arg_name);
  uint val_len = strlen(val_for_comp);

  if (val_len >= 2 && val_for_comp[0] == '-' && val_for_comp[1] == short_name &&
      (val_len == 2 || val_for_comp[2] == '=')) {
    return 0;
  }

  if (arg_len + 2 /*first `--`*/ > val_len) {
    return 1;
  }

  if (val_for_comp[0] != '-' && val_for_comp[1] != '-') {
    return 2;
  }

  if (val_len > arg_len + 2 && val_for_comp[arg_len + 2] != '=') {
    return 3;
  }

  if (str_arg_cmp(arg_name, val_for_comp + 2) != 0) {
    return 4;
  }

  return 0;
}

inline char *arg_parser_usage(arg_parser *parser) {
  char **list_fmt_args   = NULL;
  char * retval          = NULL;
  uint   count           = 0;
  uint   longest_fmt_arg = 0;
  uint   desc_len        = 0;
  uint   usage_len       = 0;
  uint   retval_len      = 0;
  uint   offset          = 0;

  list_fmt_args = (char **)malloc(sizeof(char *) * parser->asize);
  for (uint i = 0; i < parser->asize; ++i) {
    list_fmt_args[i] = (char *)malloc(ARG_MAX_FMT_ARG_LEN);
  }

  for (uint i = 0; i < parser->asize; ++i) {
    arg_desc *  arg      = &parser->alist[i];
    const char *arg_name = arg->name;
    if (parser->alist[i].shrt && arg->flgs & ArgDefault) {
      char *arg_def = val_to_string(arg->dval, arg->type);
      count         = snprintf(list_fmt_args[i],
                       ARG_MAX_FMT_ARG_LEN,
                       "  -%c, --%s (=%s)",
                       arg->shrt,
                       arg_name,
                       arg_def);
      free(arg_def);
    } else if (arg->flgs & ArgDefault) {
      char *arg_def = val_to_string(arg->dval, arg->type);
      count         = snprintf(list_fmt_args[i],
                       ARG_MAX_FMT_ARG_LEN,
                       "      --%s (=%s)",
                       arg_name,
                       arg_def);
      free(arg_def);
    } else if (parser->alist[i].shrt) {
      count = snprintf(list_fmt_args[i],
                       ARG_MAX_FMT_ARG_LEN,
                       "  -%c, --%s",
                       arg->shrt,
                       arg_name);
    } else {
      count = snprintf(list_fmt_args[i],
                       ARG_MAX_FMT_ARG_LEN,
                       "      --%s",
                       arg_name);
    }

    if (count >= ARG_MAX_FMT_ARG_LEN) {
      list_fmt_args[i][ARG_MAX_FMT_ARG_LEN - 1] = '\0';
    }
  }


  for (uint i = 0; i < parser->asize; ++i) {
    count = strlen(list_fmt_args[i]);
    if (longest_fmt_arg < count) {
      longest_fmt_arg = count;
    }

    desc_len += strlen(parser->alist[i].desc);
  }

  usage_len  = strlen(parser->mdesc);
  usage_len  = usage_len > 0 ? usage_len + 1 /*new line*/ : 0;
  retval_len = usage_len + parser->asize * longest_fmt_arg +
               parser->asize * 2 /*space and new line*/ + desc_len + 1 /*\0*/;

  retval                 = (char *)malloc(retval_len);
  retval[retval_len - 1] = '\0';


  if (usage_len) {
    offset +=
        snprintf(retval + offset, retval_len - offset, "%s\n", parser->mdesc);
  }
  for (uint i = 0; i < parser->asize; ++i) {
    count =
        snprintf(retval + offset, retval_len - offset, "%s", list_fmt_args[i]);

    memset(retval + offset + count, ' ', longest_fmt_arg - count);
    offset += longest_fmt_arg;

    offset += snprintf(retval + offset,
                       retval_len - offset,
                       " %s\n",
                       parser->alist[i].desc);

    assert(count <= longest_fmt_arg && "wrong arg size calculation");
    assert(offset < retval_len && "wrong complete size calculation");
  }


  for (uint i = 0; i < parser->asize; ++i) {
    free(list_fmt_args[i]);
  }
  free(list_fmt_args);


  return retval;
}


inline void arg_parser_add_arg(arg_parser *   parser,
                               const char *   name,
                               char           short_name,
                               const char *   desc,
                               enum ArgType   type,
                               int            flags,
                               union ArgUnion default_val) {
  parser->alist =
      (arg_desc *)realloc(parser->alist, sizeof(arg_desc) * ++parser->asize);

  char *desc_copy;
  if (desc) {
    desc_copy = (char *)malloc(strlen(desc) + 1);
    strcpy(desc_copy, desc);
  } else {
    desc_copy    = (char *)malloc(1);
    desc_copy[0] = '\0';
  }

  arg_desc arg =
      {str_to_arg_name(name), desc_copy, default_val, type, flags, short_name};

  parser->alist[parser->asize - 1] = arg;
}

inline arg_parser *arg_parser_make(const char *main_desc) {
  arg_parser *retval = (arg_parser *)malloc(sizeof(arg_parser));
  retval->alist      = NULL;
  retval->rlist      = NULL;
  retval->asize      = 0;
  retval->rsize      = 0;
  if (main_desc) {
    retval->mdesc = (char *)malloc(strlen(main_desc) + 1);
    strcpy(retval->mdesc, main_desc);
  } else {
    retval->mdesc    = (char *)malloc(1);
    retval->mdesc[0] = '\0';
  }
  return retval;
}

inline void arg_parser_dispose(arg_parser *parser) {
  for (uint i = 0; i < parser->asize; ++i) {
    free(parser->alist[i].name);
    free(parser->alist[i].desc);
  }
  free(parser->mdesc);
  free(parser->alist);
  free(parser->rlist);
  parser->mdesc = 0;
  parser->alist = NULL;
  parser->rlist = NULL;
  parser->asize = 0;
  parser->rsize = 0;

  free(parser);
}


#define ARG_PARSER_ERROR(ret_ptr, err_buf, ...)                        \
  if (ret_ptr) {                                                       \
    int ret_count = snprintf(err_buf, ARG_MAX_ERROR_LEN, __VA_ARGS__); \
    if (ret_count >= ARG_MAX_ERROR_LEN) {                              \
      err_buf[ARG_MAX_ERROR_LEN - 1] = '\0';                           \
      ret_count                      = ARG_MAX_ERROR_LEN;              \
    }                                                                  \
    *ret_ptr = (char *)malloc(ret_count + 1);                          \
    strcpy(*ret_ptr, err_buf);                                         \
  }

inline int arg_parser_parse(arg_parser *parser,
                            int *       argc,
                            char **     argv[],
                            bool        ignore_not_defined_flags,
                            bool        remove_defined_flags_from_argv,
                            char **     err) {
  char        err_buf[ARG_MAX_ERROR_LEN];
  arg_desc *  arg     = NULL;
  arg_rval *  val     = NULL;
  const char *flag    = NULL;
  const char *retval  = NULL;
  char *      endval  = NULL;
  int         counter = 0;

  for (int val_iter = 1; val_iter < *argc; val_iter += counter) {
    counter = 1;
    flag    = (*argv)[val_iter];
    if (flag[0] != '-') {
      continue; // positional arg, ignore
    }


    bool found = false;
    for (uint arg_iter = 0; arg_iter < parser->asize; ++arg_iter) {
      arg = &parser->alist[arg_iter];
      if (arg_name_cmp(arg->name, arg->shrt, flag) == 0) {
        retval = strstr(flag, "=");

        if (retval == NULL && val_iter == *argc - 1 && arg->type != ArgBool) {
          goto NoValueForFlag;
        }

        if (retval != NULL) {
          ++retval; // ignore `=` symbol
        } else if (arg->type != ArgBool) {
          retval = (*argv)[val_iter + 1];
          ++counter;
        } else {
          if (val_iter == *argc - 1 || (*argv)[val_iter + 1][0] == '-') {
            retval = "true";
          } else {
            retval = (*argv)[val_iter + 1];
            ++counter;
          }
        }

        parser->rlist = (arg_rval *)realloc(parser->rlist,
                                            sizeof(arg_rval) * ++parser->rsize);
        val           = &parser->rlist[parser->rsize - 1];
        val->name     = arg->name;
        val->type     = arg->type;

        switch (arg->type) {
        case ArgString:
          val->rval.val_str = retval;
          break;
        case ArgBool:
          if (strcmp(retval, "true") == 0) {
            val->rval.val_bool = true;
          } else if (strcmp(retval, "false") == 0) {
            val->rval.val_bool = false;
          } else {
            val->rval.val_bool = strtol(retval, &endval, 0);
            if (endval != retval + strlen(retval)) {
              goto ConversionError;
            }
          }
          break;
        case ArgInt:
          val->rval.val_int = strtol(retval, &endval, 0);
          if (endval != retval + strlen(retval)) {
            goto ConversionError;
          }
          break;
        case ArgLong:
          val->rval.val_long = strtol(retval, &endval, 0);
          if (endval != retval + strlen(retval)) {
            goto ConversionError;
          }
          break;
        case ArgLongLong:
          val->rval.val_ll = strtoll(retval, &endval, 0);
          if (endval != retval + strlen(retval)) {
            goto ConversionError;
          }
          break;
        case ArgDouble:
          val->rval.val_double = strtod(retval, &endval);
          if (endval != retval + strlen(retval)) {
            goto ConversionError;
          }
          break;
        }

        found = true;
        arg->flgs |= ArgFound;
        break;
      }
    }

    if (found == false && ignore_not_defined_flags == false) {
      goto NotDefinedFlagFound;
    }

    if (remove_defined_flags_from_argv) {
      for (int i = val_iter; i < (*argc - counter); ++i) {
        (*argv)[i] = (*argv)[i + counter];
      }
      (*argc) -= counter;
      counter = 0;
    }
  }

  for (uint arg_iter = 0; arg_iter < parser->asize; ++arg_iter) {
    arg = &parser->alist[arg_iter];
    if ((arg->flgs & ArgFound) == 0) {
      if (arg->flgs & ArgDefault) {
        parser->rlist = (arg_rval *)realloc(parser->rlist,
                                            sizeof(arg_rval) * ++parser->rsize);
        parser->rlist[parser->rsize - 1].name = arg->name;
        parser->rlist[parser->rsize - 1].type = arg->type;
        parser->rlist[parser->rsize - 1].rval = arg->dval;
      } else if (arg->flgs & ArgRequired) {
        goto FlagNotFound;
      }
    }
  }

  return 0;

NoValueForFlag:
  ARG_PARSER_ERROR(err, err_buf, "no value for %s", flag);
  return 1;

NotDefinedFlagFound:
  ARG_PARSER_ERROR(err, err_buf, "unknown flag: %s", flag);
  return 2;

FlagNotFound:
  ARG_PARSER_ERROR(err, err_buf, "can't find required flag: --%s", arg->name);
  return 3;

ConversionError:
  ARG_PARSER_ERROR(err, err_buf, "can't convert: %s %s", flag, retval);
  return 4;
}

inline int arg_parser_count(arg_parser *parser, const char *name) {
  int  count    = 0;
  uint name_len = strlen(name);
  for (uint i = 0; i < parser->rsize; ++i) {
    arg_rval *  arg      = &parser->rlist[i];
    const char *arg_name = arg->name;
    if (strlen(arg_name) == name_len && str_arg_cmp(arg_name, name) == 0) {
      ++count;
    }
  }

  return count;
}

inline int arg_parser_get_args(const arg_parser *parser,
                               const char *      name,
                               enum ArgType      type,
                               void *            val,
                               int               count) {
  int  retval   = 0;
  uint name_len = strlen(name);
  for (uint i = 0; i < parser->rsize && retval < count; ++i) {
    arg_rval *  arg      = &parser->rlist[i];
    const char *arg_name = arg->name;
    if (strlen(arg_name) == name_len && str_arg_cmp(arg_name, name) == 0) {
      if (type != arg->type) {
        return 1;
      }
      switch (type) {
      case ArgString:
        *((const char **)val + retval++) = arg->rval.val_str;
        break;
      case ArgBool:
        *((bool *)val + retval++) = arg->rval.val_bool;
        break;
      case ArgInt:
        *((int *)val + retval++) = arg->rval.val_int;
        break;
      case ArgLong:
        *((long *)val + retval++) = arg->rval.val_long;
        break;
      case ArgLongLong:
        *((long long *)val + retval++) = arg->rval.val_ll;
        break;
      case ArgDouble:
        *((double *)val + retval++) = arg->rval.val_double;
        break;
      default:
        assert(false && "unknown arg type");
      }
    }
  }

  return retval;
}


inline union ArgUnion arg_union_make_from_str(const char *val) {
  union ArgUnion retval;
  retval.val_str = val;
  return retval;
}

inline union ArgUnion arg_union_make_from_bool(bool val) {
  union ArgUnion retval;
  retval.val_bool = val;
  return retval;
}

inline union ArgUnion arg_union_make_from_int(int val) {
  union ArgUnion retval;
  retval.val_int = val;
  return retval;
}

inline union ArgUnion arg_union_make_from_long(long val) {
  union ArgUnion retval;
  retval.val_long = val;
  return retval;
}

inline union ArgUnion arg_union_make_from_ll(long long val) {
  union ArgUnion retval;
  retval.val_ll = val;
  return retval;
}

inline union ArgUnion arg_union_make_from_double(double val) {
  union ArgUnion retval;
  retval.val_double = val;
  return retval;
}

inline void *void_ptr_cast_from_str(const char **val) {
  return (union ArgUnion *)val;
}

inline void *void_ptr_cast_from_bool(bool *val) {
  return (union ArgUnion *)val;
}

inline void *void_ptr_cast_from_int(int *val) {
  return (union ArgUnion *)val;
}

inline void *void_ptr_cast_from_long(long *val) {
  return (union ArgUnion *)val;
}

inline void *void_ptr_cast_from_ll(long long *val) {
  return (union ArgUnion *)val;
}

inline void *void_ptr_cast_from_double(double *val) {
  return (union ArgUnion *)val;
}

#ifdef __cplusplus
}
#endif
