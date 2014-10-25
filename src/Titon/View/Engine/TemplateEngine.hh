<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\Engine;

use Titon\Common\DataMap;

/**
 * Standard engine used for rendering views using pure PHP code.
 *
 * @package Titon\View\Engine
 */
class TemplateEngine extends AbstractEngine {

    /**
     * {@inheritdoc}
     */
    public function render(string $path, DataMap $variables = Map {}): string {
        $this->_variables = $variables;

        if ($variables) {
            extract($variables->toArray(), EXTR_OVERWRITE);
        }

        ob_start();

        include $path;

        return ob_get_clean();
    }

}