<?php
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

    const SUNDAY = 0;
    const MONDAY = 1;
    const TUESDAY = 2;
    const WEDNESDAY = 3;
    const THURSDAY = 4;
    const FRIDAY = 5;
    const SATURDAY = 6;

    /**
     * Initialize mappings.
     *
     * @type array
     */
    protected $_enums = [
        self::SUNDAY    => ['sunday'],
        self::MONDAY    => ['monday'],
        self::TUESDAY   => ['tuesday'],
        self::WEDNESDAY => ['wednesday'],
        self::THURSDAY  => ['thursday'],
        self::FRIDAY    => ['friday'],
        self::SATURDAY  => ['saturday']
    ];

    /**
     * Day of the week; Sunday is first.
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
     * Day as a slug.
     *
     * @type string
     */
    public $slug;

    /**
     * Current day of the year.
     *
     * @type int
     */
    public $dayOfYear;

    /**
     * Set variables.
     *
     * @param string $slug
     */
    public function initialize($slug) {
        $time = strtotime($slug);

        $this->order = $this->value();
        $this->slug = $slug;
        $this->name = strftime('%A', $time);
        $this->shortName = strftime('%a', $time);
        $this->dayOfYear = date('z', $time);
    }

}