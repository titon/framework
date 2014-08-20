<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io;

/**
 * Interface for the file readers library.
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
     * @return Map<string, mixed>
     */
    public function read(): Map<string, mixed>;

    /**
     * Reset the file path.
     *
     * @param string $path
     * @return $this
     */
    public function reset(string $path = ''): this;

}