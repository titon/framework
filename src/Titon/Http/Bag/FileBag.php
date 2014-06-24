<?php
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Bag;

use Titon\Common\Bag\AbstractBag;
use Titon\Http\RequestAware;
use Titon\Utility\Hash;

/**
 * Bag for interacting with session parameters.
 *
 * @package Titon\Http\Bag
 */
class FileBag extends AbstractBag {
    use RequestAware;

    /**
     * Set and normalize the FILES array.
     *
     * @param array $files
     */
    public function __construct(array $files = []) {
        $this->_data = $this->normalize($files);
    }

    /**
     * Normalize a multi-dimensional array in case nested arrays have been used.
     *
     * @param array $files
     * @return array
     */
    public function normalize(array $files) {
        if ($flatFiles = Hash::flatten($files)) {
            $files = [];

            foreach ($flatFiles as $key => $value) {
                if (preg_match('/\.(?:name|type|tmp_name|error|size)/', $key, $matches)) {
                    $key = str_replace($matches[0], '', $key);
                    $key .= $matches[0];
                }

                $files = Hash::set($files, $key, $value);
            }
        }

        return $files;
    }

}