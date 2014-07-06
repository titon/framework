<?hh //strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Type\Enum;

use Titon\Type\Enum;

/**
 * Enum for months of the year.
 *
 * @package Titon\Type\Enum
 */
class Month extends Enum {

    const JANUARY = 0;
    const FEBRUARY = 1;
    const MARCH = 2;
    const APRIL = 3;
    const MAY = 4;
    const JUNE = 5;
    const JULY = 6;
    const AUGUST = 7;
    const SEPTEMBER = 8;
    const OCTOBER = 9;
    const NOVEMBER = 10;
    const DECEMBER = 11;

    /**
     * Initialize mappings.
     *
     * @type array
     */
    protected $_enums = [
        self::JANUARY   => ['january', 31],
        self::FEBRUARY  => ['february', 28],
        self::MARCH     => ['march', 31],
        self::APRIL     => ['april', 30],
        self::MAY       => ['may', 31],
        self::JUNE      => ['june', 30],
        self::JULY      => ['july', 31],
        self::AUGUST    => ['august', 31],
        self::SEPTEMBER => ['september', 30],
        self::OCTOBER   => ['october', 31],
        self::NOVEMBER  => ['november', 30],
        self::DECEMBER  => ['december', 31]
    ];

    /**
     * Month of the year.
     *
     * @type int
     */
    public $order;

    /**
     * Localized name.
     *
     * @type string
     */
    public $name;

    /**
     * Localized shorthand name.
     *
     * @type string
     */
    public $shortName;

    /**
     * Month as a slug.
     *
     * @type string
     */
    public $slug;

    /**
     * How many days in the month.
     *
     * @type int
     */
    public $daysInMonth;

    /**
     * Set variables.
     *
     * @param string $slug
     * @param int $daysInMonth
     */
    public function initialize($slug, $daysInMonth = 0) {
        $month = $this->value() + 1;
        $time = mktime(0, 0, 0, $month, 1);

        $this->order = $month;
        $this->slug = $slug;
        $this->name = strftime('%B', $time);
        $this->shortName = strftime('%b', $time);
        $this->daysInMonth = date('t', $time) ?: $daysInMonth;
    }

}