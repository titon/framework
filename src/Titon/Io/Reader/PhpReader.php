<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Reader;

use Titon\Io\Reader\AbstractReader;
use Titon\Io\Exception\ReadErrorException;

/**
 * A file reader that parses PHP files.
 * The PHP file must contain a return statement that returns an array.
 *
 * @package Titon\Io\Reader
 */
class PhpReader extends AbstractReader {

    /**
     * {@inheritdoc}
     *
     * @throws \Titon\Io\Exception\ReadErrorException
     */
    public function read() {
        return $this->cache([__METHOD__, $this->path()], function() {
            if ($this->exists()) {
                return include $this->path();
            }

            throw new ReadErrorException(sprintf('PhpReader failed to parse %s', $this->name()));
        });
    }

}