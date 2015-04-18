<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\G11n\Translator;

use Titon\Common\Config;
use Titon\Common\Registry;
use Titon\G11n\G11n;
use Titon\G11n\Exception\MissingMessageException;
use Titon\G11n\Translator\AbstractTranslator;
use Titon\G11n\Translator;
use Titon\Utility\String;
use \Locale;

/**
 * Translator used for hooking into the GNU gettext library and fetching messages from locale domain files.
 *
 * @package Titon\G11n\Translator
 */
class GettextTranslator extends AbstractTranslator {

    /**
     * {@inheritdoc}
     *
     * @uses Titon\Common\Registry
     * @uses Titon\Utility\String
     */
    public function bindDomains($domain, $catalog) {
        bind_textdomain_codeset($catalog, Config::encoding());

        return $this->cache([__METHOD__, $domain, $catalog], function() use ($domain, $catalog) {
            $locations = G11n::registry()->current()->getMessageBundle()->getPaths();

            foreach ($locations as $location) {
                bindtextdomain($catalog, $location);
            }

            return true;
        });
    }

    /**
     * {@inheritdoc}
     *
     * @throws \Titon\G11n\Exception\MissingMessageException
     */
    public function getMessage($key) {
        return $this->cache([__METHOD__, $key], function(GettextTranslator $translator) use ($key) {
            list($domain, $catalog, $id) = $translator->parseKey($key);

            $translator->bindDomains($domain, $catalog);

            textdomain($catalog);

            $message = gettext($id);

            if ($message !== $id) {
                return $message;
            }

            throw new MissingMessageException(sprintf('Message key %s does not exist in %s', $key, Locale::DEFAULT_LOCALE));
        });
    }

}
