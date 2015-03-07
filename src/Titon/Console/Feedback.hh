<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

interface Feedback {

    public function advance(): void;

    public function finish(): void;

    public function setMessage(string $message): this;

    public function setPrefix(string $prefix): this;

    public function setSuffix(string $suffix): this;

    public function setTotal(int $total): this;
}