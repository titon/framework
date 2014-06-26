<?php
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\View\Engine;

/**
 * Standard engine used for rendering views using pure PHP code.
 *
 * @package Titon\View\View\Engine
 */
class TemplateEngine extends AbstractEngine {

    /**
     * {@inheritdoc}
     */
    public function render($path, array $variables = []) {
        $this->_variables = $variables;

        if ($variables) {
            extract($variables, EXTR_OVERWRITE);
        }

        ob_start();

        include $path;

        return ob_get_clean();
    }

}