<?hh // partial
// Because of `include`.
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\Engine;

use Titon\Common\DataMap;

/**
 * Standard engine used for rendering templates. Uses raw PHP code.
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