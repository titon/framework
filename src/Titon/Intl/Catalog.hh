<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Intl;

use Titon\Intl\Exception\InvalidMessageKeyException;
use Titon\Intl\Exception\MissingMessageException;

/**
 * The `Catalog` represents an accumulation of message strings for a specific domain and catalog
 * from a list of defined resource bundles.
 *
 * @package Titon\Intl
 */
class Catalog {

    /**
     * Domain the catalog belongs to.
     *
     * @var string
     */
    protected string $domain;

    /**
     * List of messages within the catalog.
     *
     * @var \Titon\Intl\MessageMap
     */
    protected MessageMap $messages = Map {};

    /**
     * Name of the catalog.
     *
     * @var string
     */
    protected string $name;

    /**
     * Store the domain and catalog name and message strings.
     *
     * @param string $name
     * @param string $domain
     * @param \Titon\Intl\MessageMap $messages
     */
    public function __construct(string $name, string $domain, MessageMap $messages) {
        $this->name = $name;
        $this->domain = $domain;
        $this->messages = $messages;
    }

    /**
     * Return the domain name.
     *
     * @return string
     */
    public function getDomain(): string {
        return $this->domain;
    }

    /**
     * Return a message from the catalog defined by key.
     *
     * @param string $key
     * @return string
     * @throws \Titon\Intl\Exception\MissingMessageException
     */
    public function getMessage(string $key): string {
        if ($this->messages->contains($key)) {
            return $this->messages[$key];
        }

        throw new MissingMessageException(sprintf('Message key %s.%s.%s does not exist', $this->getDomain(), $this->getName(), $key));
    }

    /**
     * Return the list of messages.
     *
     * @return \Titon\Intl\MessageMap
     */
    public function getMessages(): MessageMap {
        return $this->messages;
    }

    /**
     * Return the catalog name.
     *
     * @return string
     */
    public function getName(): string {
        return $this->name;
    }

    /**
     * Parse out the domain, catalog, and key for a message lookup.
     *
     * @param string $key
     * @return \Titon\Intl\MessageKey
     * @throws \Titon\Intl\Exception\InvalidMessageKeyException
     */
    <<__Memoize>>
    public static function parseKey(string $key): MessageKey {
        $parts = explode('.', preg_replace('/[^-_a-z0-9\.]+/i', '', $key), 3);
        $count = count($parts);
        $domain = MessageLoader::DEFAULT_DOMAIN;
        $catalog = MessageLoader::DEFAULT_CATALOG;

        if (!$key || $count === 0) {
            throw new InvalidMessageKeyException(sprintf('No domain or catalog present for %s key', $key));

        } else if ($count === 1) {
            $key = $parts[0];

        } else if ($count === 2) {
            $catalog = $parts[0];
            $key = $parts[1];

        } else {
            $domain = $parts[0];
            $catalog = $parts[1];
            $key = $parts[2];
        }

        return shape(
            'domain' => $domain,
            'catalog' => $catalog,
            'id' => $key
        );
    }

}
