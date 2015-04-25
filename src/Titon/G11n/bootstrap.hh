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

namespace Titon\G11n {
    type LocaleMap = Map<string, Locale>;
    type LocaleTagMap = Map<string, string>;
    type MessageKey = shape('domain' => string, 'catalog' => string, 'key' => string);
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

    /**
     * Convenience function for fetching a localized string.
     * Uses a single combination key.
     *
     * @uses Titon\Common\Registry
     *
     * @param string $key
     * @param array $params
     * @return string
     */
    function msg($key, array $params = []) {
        return G11n::registry()->translate($key, $params);
    }

    /**
     * Convenience function for fetching a localized string.
     * Uses separate values for key.
     *
     * @uses Titon\Common\Registry
     *
     * @param string $id
     * @param string $catalog
     * @param string $domain
     * @param array $params
     * @return string
     */
    function __($id, $catalog = 'default', $domain = 'core', array $params = []) {
        return G11n::registry()->translate(sprintf('%s.%s.%s', $domain, $catalog, $id), $params);
    }

}
