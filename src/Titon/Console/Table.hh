<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

interface Table {

    public function addRow(Vector<string> $row): this;

    public function render(): string;

    public function setRows(Vector<Vector<string>> $rows): this;
}