#include <map>

#ifndef SASS_NODE_INCLUDED
#include "node.hpp"
#endif

#include "context.hpp"

namespace Sass {
  using std::string;
  using std::vector;
  using std::map;
  using namespace Prelexer;

  struct Document {
    enum CSS_Style { nested, expanded, compact, compressed, echo };
    
    string path;
    char* source;
    const char* position;
    size_t line_number;
    bool own_source;
    
    // TO DO: move the environment up into the context class when it's ready
    // map<Token, Node> environment;
    
    Context& context;
    
    Node root;
    // vector<Node> statements;
    Token lexed;
    
    Document(string path, char* source = 0);
    Document(string path, Context& context);
    ~Document();
    
    template <prelexer mx>
    const char* peek(const char* start = 0)
    {
      if (!start) start = position;
      const char* after_whitespace;
      if (mx == block_comment) {
        after_whitespace =
          zero_plus< alternatives<spaces, line_comment> >(start);
      }
      else if (/*mx == ancestor_of ||*/ mx == no_spaces) {
        after_whitespace = position;
      }
      else if (mx == spaces || mx == ancestor_of) {
        after_whitespace = mx(start);
        if (after_whitespace) {
          return after_whitespace;
        }
        else {
          return 0;
        }
      }
      else if (mx == optional_spaces) {
        after_whitespace = optional_spaces(start);
      }
      else {
        after_whitespace = spaces_and_comments(start);
      }
      const char* after_token = mx(after_whitespace);
      if (after_token) {
        return after_token;
      }
      else {
        return 0;
      }
    }
    
    template <prelexer mx>
    const char* lex()
    {
      const char* after_whitespace;
      if (mx == block_comment) {
        after_whitespace =
          zero_plus< alternatives<spaces, line_comment> >(position);
      }
      else if (mx == ancestor_of || mx == no_spaces) {
        after_whitespace = position;
      }
      else if (mx == spaces) {
        after_whitespace = spaces(position);
        if (after_whitespace) {
          line_number += count_interval<'\n'>(position, after_whitespace);
          lexed = Token(position, after_whitespace);
          return position = after_whitespace;
        }
        else {
          return 0;
        }
      }
      else if (mx == optional_spaces) {
        after_whitespace = optional_spaces(position);
      }
      else {
        after_whitespace = spaces_and_comments(position);
      }
      const char* after_token = mx(after_whitespace);
      if (after_token) {
        line_number += count_interval<'\n'>(position, after_token);
        lexed = Token(after_whitespace, after_token);
        return position = after_token;
      }
      else {
        return 0;
      }
    }
    
    void parse_scss();
    Node parse_import();
    Node parse_include(bool delay = false);
    void parse_mixin_def();
    Node parse_parameter();
    Node parse_arguments(bool delay = false);
    Node parse_mixin_params();
    Node parse_var_def(bool delay = false);
    Node parse_ruleset();
    Node parse_selector_group();
    Node parse_selector();
    Node parse_simple_selector_sequence();
    Node parse_simple_selector();
    Node parse_pseudo();
    Node parse_attribute_selector();
    Node parse_block(bool delay = false);
    Node parse_rule(bool delay = false);
    Node parse_values();
    Node parse_list(bool delay = false);
    Node parse_comma_list(bool delay = false);
    Node parse_space_list(bool delay = false);
    Node parse_expression(bool delay = false);
    Node parse_term(bool delay = false);
    Node parse_factor(bool delay = false);
    Node parse_value(bool delay = false);
    
    const char* look_for_rule(const char* start = 0);
    const char* look_for_values(const char* start = 0);
    
    const char* look_for_selector_group(const char* start = 0);
    const char* look_for_selector(const char* start = 0);
    const char* look_for_simple_selector_sequence(const char* start = 0);
    const char* look_for_simple_selector(const char* start = 0);
    const char* look_for_pseudo(const char* start = 0);
    const char* look_for_attrib(const char* start = 0);
    
    string emit_css(CSS_Style style);

  };
}