<?hh
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

/**
 * Add G11n resources if VENDOR_DIR constant exists.
 */
if (defined('VENDOR_DIR')) {
    //Config::add('titon.path.resources', VENDOR_DIR . '/titon/g11n/src/resources/');
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
