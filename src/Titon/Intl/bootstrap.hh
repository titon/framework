<?hh
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

/**
 * --------------------------------------------------------------
 *  Resource Paths
 * --------------------------------------------------------------
 *
 * Defines default resource paths for the locales and messages
 * provided by the globalization package.
 */

namespace {
    use Titon\Utility\Config;

    Config::add('titon.paths.resources', __DIR__ . '/');
    Config::add('titon.paths.locales', __DIR__ . '/locales/');
    Config::add('titon.paths.messages', __DIR__ . '/messages/');
}

/**
 * --------------------------------------------------------------
 *  Type Aliases
 * --------------------------------------------------------------
 *
 * Defines type aliases that are used by the HTTP package.
 */

namespace Titon\Intl {
    type CatalogMap = Map<string, Catalog>;
    type LocaleMap = Map<string, Locale>;
    type LocaleTagMap = Map<string, string>;
    type MessageKey = shape('domain' => string, 'catalog' => string, 'key' => string);
    type MessageMap = Map<string, string>;
    type ParamList = Vector<mixed>;
}

/**
 * --------------------------------------------------------------
 *  Helper Functions
 * --------------------------------------------------------------
 *
 * Defines global helper functions for common use cases.
 */

namespace {
    use Titon\Context\Depository;
    use Titon\Intl\Translator;

    /**
     * Helper function for fetching a localized string using separate key parts.
     *
     * @param string $id
     * @param string $catalog
     * @param string $domain
     * @param \Titon\Intl\ParamList $params
     * @return string
     */
    function __($id, $catalog = 'default', $domain = 'common', ParamList $params = Vector {}): string {
        return msg(sprintf('%s.%s.%s', $domain, $catalog, $id), $params);
    }

    /**
     * Helper function for fetching a localized string using a single combination key.
     *
     * @uses Titon\Common\Registry
     *
     * @param string $key
     * @param \Titon\Intl\ParamList $params
     * @return string
     */
    function msg(string $key, ParamList $params = Vector {}): string {
        return translator()->translate($key, $params);
    }

    /**
     * Make and return a Translator instance from the depository.
     *
     * @return \Titon\Intl\Translator
     */
    function translator(): Translator {
        $translator = Depository::getInstance()->make('Titon\Intl\Translator');

        invariant($translator instanceof Translator, 'Must be a Translator.');

        return $translator;
    }

}
