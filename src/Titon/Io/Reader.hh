<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io;

use Titon\Io\ResourceMap;

/**
 * Interface for reading file resources.
 *
 * @package Titon\Io
 */
interface Reader {

    /**
     * Return the current path.
     *
     * @return string
     */
    public function path(): string;

    /**
     * Read the file contents.
     *
     * @return \Titon\Io\ResourceMap
     */
    public function read<Tk, Tv>(): Map<Tk, Tv>;

    /**
     * Reset the file path.
     *
     * @param string $path
     * @return $this
     */
    public function reset(string $path = ''): this;

}