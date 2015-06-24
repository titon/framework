<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Route;

/**
 * Prepends all routes with a locale matching pattern.
 *
 * @package Titon\Route
 */
class LocaleRoute extends Route {

    /**
     * Prepend the locale to the front of the route before compilation.
     *
     * @return string
     */
    public function compile(): string {
        if ($this->isCompiled()) {
            return $this->compiled;
        }

        if (mb_substr($this->getPath(), 0, 9) !== '/<locale>') {
            $this->prepend('/<locale>');
        }

        $this->addPattern('locale', self::LOCALE);

        return parent::compile();
    }

}
