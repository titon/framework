<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io;

use Titon\Common\DataMap;

/**
 * Interface for the file writers library.
 *
 * @package Titon\Io
 */
interface Writer {

    /**
     * Append data to the end of a file.
     *
     * @param \Titon\Common\DataMap $data
     * @return bool
     */
    public function append(DataMap $data): bool;

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
     * @param \Titon\Common\DataMap $data
     * @return bool
     */
    public function write(DataMap $data): bool;

}