<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Utility\State;

use Titon\Utility\Col;

/**
 * The Files class acts as a static immutable wrapper for the $_FILES super global.
 *
 * @package Titon\Utility\State
 */
final class Files extends Request {

    /**
     * Normalize a multi-dimensional array for situations where nested file names have been used.
     *
     * @param array<string, mixed> $data
     * @return \Titon\Utility\State\GlobalMap
     */
    public static function package(array<string, mixed> $data): GlobalMap {
        $map = Map {};

        if ($files = Col::flatten($data)) {
            $matches = [];

            foreach ($files as $key => $value) {
                if (preg_match('/\.(?:name|type|tmp_name|error|size)/', $key, $matches)) {
                    $key = str_replace($matches[0], '', $key) . $matches[0];
                }

                $map = Col::set($map, $key, $value);
            }
        }

        return $map;
    }

}