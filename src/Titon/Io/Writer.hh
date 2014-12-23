<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io;

/**
 * Interface for the file writers library.
 *
 * @package Titon\Io
 */
interface Writer {

    /**
     * Append data to the end of a file.
     *
     * @param string $data
     * @return bool
     */
    public function append(string $data): bool;

    /**
     * Return the current path.
     *
     * @return string
     */
    public function path(): string;

    /**
     * Reset the file path.
     *
     * @param string $path
     * @return $this
     */
    public function reset(string $path = ''): this;

    /**
     * Truncate source file and write data to it.
     *
     * @param string $data
     * @return bool
     */
    public function write(string $data): bool;

}