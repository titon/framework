<?hh
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

/**
 * --------------------------------------------------------------
 *  Type Aliases
 * --------------------------------------------------------------
 *
 * Defines type aliases that are used by the validate package.
 */

namespace Titon\Validate {
    type ConstraintCallback = (function(mixed): bool);
    type ConstraintMap = Map<string, ConstraintCallback>;
    type ErrorMap = Map<string, string>;
    type FieldMap = Map<string, string>;
    type MessageMap = Map<string, string>;
    type Rule = shape('rule' => string, 'message' => string, 'options' => OptionList);
    type RuleContainer = Map<string, RuleMap>;
    type RuleMap = Map<string, Rule>;
    type OptionList = Vector<mixed>;
}
