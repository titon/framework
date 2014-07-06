<?hh //strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Type\Enum;

use Titon\Type\Enum;

/**
 * Enum for primary and secondary colors.
 *
 * @package Titon\Type\Enum
 */
class Color extends Enum {

    const BLACK = 0;
    const WHITE = 1;
    const YELLOW = 2;
    const GREEN = 3;
    const BLUE = 4;
    const RED = 5;
    const ORANGE = 6;
    const PURPLE = 7;
    const PINK = 8;
    const GRAY = 9;
    const BROWN = 10;
    const TEAL = 10;

    /**
     * Initialize mappings.
     *
     * @type array
     */
    protected $_enums = [
        self::BLACK     => ['000000', 0, 0, 0],
        self::WHITE     => ['FFFFFF', 255, 255, 255],
        self::YELLOW    => ['FFFF00', 255, 255, 0],
        self::GREEN     => ['00FF00', 0, 255, 0],
        self::BLUE      => ['0000FF', 0, 0, 255],
        self::RED       => ['FF0000', 255, 0, 0],
        self::ORANGE    => ['FF4500', 255, 69, 0],
        self::PURPLE    => ['800080', 128, 0, 128],
        self::PINK      => ['FF1493', 255, 20, 147],
        self::GRAY      => ['808080', 128, 128, 128],
        self::BROWN     => ['D2B48C', 210, 180, 140],
        self::TEAL      => ['008080', 0, 128, 128]
    ];

    /**
     * Hex code.
     *
     * @type string
     */
    public $hex;

    /**
     * Red value.
     *
     * @type int
     */
    public $r;

    /**
     * Green value.
     *
     * @type int
     */
    public $g;

    /**
     * Blue value.
     *
     * @type int
     */
    public $b;

    /**
     * Set variables.
     *
     * @param string $hex
     * @param int $r
     * @param int $g
     * @param int $b
     */
    public function initialize($hex, $r, $g, $b) {
        $this->hex = $hex;
        $this->r = $r;
        $this->g = $g;
        $this->b = $b;
    }

}