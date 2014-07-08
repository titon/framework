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

    const int BLACK = 0;
    const int WHITE = 1;
    const int YELLOW = 2;
    const int GREEN = 3;
    const int BLUE = 4;
    const int RED = 5;
    const int ORANGE = 6;
    const int PURPLE = 7;
    const int PINK = 8;
    const int GRAY = 9;
    const int BROWN = 10;
    const int TEAL = 10;

    /**
     * Initialize mappings.
     *
     * @type Map<int, Vector<mixed>>
     */
    protected Map<int, Vector<mixed>> $_enums = [
        self::BLACK     => Vector {'000000', 0, 0, 0},
        self::WHITE     => Vector {'FFFFFF', 255, 255, 255},
        self::YELLOW    => Vector {'FFFF00', 255, 255, 0},
        self::GREEN     => Vector {'00FF00', 0, 255, 0},
        self::BLUE      => Vector {'0000FF', 0, 0, 255},
        self::RED       => Vector {'FF0000', 255, 0, 0},
        self::ORANGE    => Vector {'FF4500', 255, 69, 0},
        self::PURPLE    => Vector {'800080', 128, 0, 128},
        self::PINK      => Vector {'FF1493', 255, 20, 147},
        self::GRAY      => Vector {'808080', 128, 128, 128},
        self::BROWN     => Vector {'D2B48C', 210, 180, 140},
        self::TEAL      => Vector {'008080', 0, 128, 128}
    ];

    /**
     * Hex code.
     *
     * @type string
     */
    public string $hex;

    /**
     * Red value.
     *
     * @type int
     */
    public int $r;

    /**
     * Green value.
     *
     * @type int
     */
    public int $g;

    /**
     * Blue value.
     *
     * @type int
     */
    public int $b;

    /**
     * Set variables.
     *
     * @param string $hex
     * @param int $r
     * @param int $g
     * @param int $b
     */
    public function initialize(string $hex, int $r, int $g, int $b): void {
        $this->hex = $hex;
        $this->r = $r;
        $this->g = $g;
        $this->b = $b;
    }

}