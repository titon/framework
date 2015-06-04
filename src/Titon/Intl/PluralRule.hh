<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Intl;

/**
 * Manages the pluralization rules for languages.
 *
 * @link http://www.gnu.org/software/gettext/manual/gettext.html#Plural-forms
 * @link https://developer.mozilla.org/en-US/docs/Mozilla/Localization/Localization_and_Plurals#List_of_Plural_Rules
 * @link http://localization-guide.readthedocs.org/en/latest/l10n/pluralforms.html
 *
 * @package Titon\Intl
 */
class PluralRule {

    const int RULE_1 = 1;
    const int RULE_2 = 2;
    const int RULE_3 = 3;
    const int RULE_4 = 4;
    const int RULE_5 = 5;
    const int RULE_6 = 6;
    const int RULE_7 = 7;
    const int RULE_8 = 8;
    const int RULE_9 = 9;
    const int RULE_10 = 10;
    const int RULE_11 = 11;
    const int RULE_12 = 12;
    const int RULE_13 = 13;
    const int RULE_14 = 14;
    const int RULE_15 = 15;

    /**
     * Calculate the number of plural forms based on a rule and number.
     *
     * @param int $rule
     * @param int $number
     * @return int
     */
    public static function calculate(int $rule, int $number): int {
        switch ($rule) {
            default:
            case self::RULE_1:
                return 0;

            case self::RULE_2:
                return ($number == 1) ? 0 : 1;

            case self::RULE_3:
                return ($number > 1) ? 1 : 0;

            case self::RULE_4:
                return ($number % 10 == 1 && $number % 100 != 11) ? 0
                    : (($number % 10 >= 2 && $number % 10 <= 4 && ($number % 100 < 10 || $number % 100 >= 20)) ? 1
                        : 2);

            case self::RULE_5:
                return ($number == 1) ? 0
                    : (($number >= 2 && $number <= 4) ? 1
                        : 2);

            case self::RULE_6:
                return ($number == 1) ? 0
                    : (($number == 2) ? 1
                        : 2);

            case self::RULE_7:
                return ($number % 10 == 1 && $number % 100 != 11) ? 0
                    : (($number % 10 >= 2 && ($number % 100 < 10 || $number % 100 >= 20)) ? 1
                        : 2);

            case self::RULE_8:
                return ($number % 100 == 1) ? 0
                    : (($number % 100 == 2) ? 1
                        : (($number % 100 == 3 || $number % 100 == 4) ? 2
                            : 3));

            case self::RULE_9:
                return ($number % 10 == 1) ? 0 : 1;

            case self::RULE_10:
                return ($number == 1) ? 0
                    : (($number == 0 || ($number % 100 > 1 && $number % 100 < 11)) ? 1
                        : (($number % 100 > 10 && $number % 100 < 20) ? 2
                            : 3));

            case self::RULE_11:
                return ($number == 0) ? 0
                    : (($number % 10 == 1 && $number % 100 != 11) ? 1
                        : 2);

            case self::RULE_12:
                return ($number == 1) ? 0
                    : (($number % 10 >= 2 && $number % 10 <= 4 && ($number % 100 < 12 || $number % 100 > 14)) ? 1
                        : 2);

            case self::RULE_13:
                return ($number == 1) ? 0
                    : (($number == 2) ? 1
                        : (($number == 8 || $number == 11) ? 2
                            : 3));

            case self::RULE_14:
                return ($number == 1) ? 0
                    : (($number == 0 || ($number % 100 > 0 && $number % 100 < 20)) ? 1
                        : 2);

            case self::RULE_15:
                return ($number == 0) ? 0
                    : (($number == 1) ? 1
                        : (($number == 2) ? 2
                            : (($number % 100 >= 3 && $number % 100 <= 10) ? 3
                                : (($number % 100 >= 11 && $number % 100 <= 99) ? 4
                                    : 5))));
        }
    }

}
