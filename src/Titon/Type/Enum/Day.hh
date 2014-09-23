<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Type\Enum;

use Titon\Type\Enum;

/**
 * Enum for days of the week.
 *
 * @package Titon\Type\Enum
 */
class Day extends Enum {

    const int SUNDAY = 0;
    const int MONDAY = 1;
    const int TUESDAY = 2;
    const int WEDNESDAY = 3;
    const int THURSDAY = 4;
    const int FRIDAY = 5;
    const int SATURDAY = 6;

    /**
     * Initialize mappings.
     *
     * @type Map<int, Vector<mixed>>
     */
    protected Map<int, Vector<mixed>> $_enums = Map {
        self::SUNDAY    => Vector {'sunday'},
        self::MONDAY    => Vector {'monday'},
        self::TUESDAY   => Vector {'tuesday'},
        self::WEDNESDAY => Vector {'wednesday'},
        self::THURSDAY  => Vector {'thursday'},
        self::FRIDAY    => Vector {'friday'},
        self::SATURDAY  => Vector {'saturday'}
    };

    /**
     * Day of the week; Sunday is first.
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
     * Day as a slug.
     *
     * @type string
     */
    public string $slug = '';

    /**
     * Current day of the year.
     *
     * @type int
     */
    public int $dayOfYear = 0;

    /**
     * Set variables.
     *
     * @param string $slug
     */
    public function initialize(string $slug): void {
        $time = strtotime($slug);

        $this->order = $this->value();
        $this->slug = $slug;
        $this->name = strftime('%A', $time);
        $this->shortName = strftime('%a', $time);
        $this->dayOfYear = date('z', $time);
    }

}