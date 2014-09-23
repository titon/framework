<?hh // strict
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

    const int JANUARY = 0;
    const int FEBRUARY = 1;
    const int MARCH = 2;
    const int APRIL = 3;
    const int MAY = 4;
    const int JUNE = 5;
    const int JULY = 6;
    const int AUGUST = 7;
    const int SEPTEMBER = 8;
    const int OCTOBER = 9;
    const int NOVEMBER = 10;
    const int DECEMBER = 11;

    /**
     * Initialize mappings.
     *
     * @type Map<int, Vector<mixed>>
     */
    protected Map<int, Vector<mixed>> $_enums = Map {
        self::JANUARY   => Vector {'january', 31},
        self::FEBRUARY  => Vector {'february', 28},
        self::MARCH     => Vector {'march', 31},
        self::APRIL     => Vector {'april', 30},
        self::MAY       => Vector {'may', 31},
        self::JUNE      => Vector {'june', 30},
        self::JULY      => Vector {'july', 31},
        self::AUGUST    => Vector {'august', 31},
        self::SEPTEMBER => Vector {'september', 30},
        self::OCTOBER   => Vector {'october', 31},
        self::NOVEMBER  => Vector {'november', 30},
        self::DECEMBER  => Vector {'december', 31}
    };

    /**
     * Month of the year.
     *
     * @type int
     */
    public int $order = 0;

    /**
     * Localized name.
     *
     * @type string
     */
    public string $name = '';

    /**
     * Localized shorthand name.
     *
     * @type string
     */
    public string $shortName = '';

    /**
     * Month as a slug.
     *
     * @type string
     */
    public string $slug = '';

    /**
     * How many days in the month.
     *
     * @type int
     */
    public int $daysInMonth = 0;

    /**
     * Set variables.
     *
     * @param string $slug
     * @param int $daysInMonth
     */
    public function initialize(string $slug, int $daysInMonth = 0): void {
        $month = $this->value() + 1;
        $time = mktime(0, 0, 0, $month, 1);

        $this->order = $month;
        $this->slug = $slug;
        $this->name = strftime('%B', $time);
        $this->shortName = strftime('%b', $time);
        $this->daysInMonth = date('t', $time) ?: $daysInMonth;
    }

}