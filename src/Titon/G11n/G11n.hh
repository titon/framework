<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\G11n;

use Titon\Event\EmitsEvents;
use Titon\Event\Event;
use Titon\Event\Subject;
use Titon\G11n\Exception\MissingTranslatorException;
use Titon\G11n\Exception\MissingLocaleException;
use Titon\G11n\Locale;
use Titon\G11n\Route\LocaleRoute;
use Titon\G11n\Translator;
use Titon\Io\DomainPathMap;
use Titon\Route\Router;

/**
 * The Globalization class handles all the necessary functionality for internationalization and
 * localization. This includes defining which locales to support, loading translators,
 * parsing resource bundles and initializing environments.
 *
 * @link http://en.wikipedia.org/wiki/IETF_language_tag
 * @link http://en.wikipedia.org/wiki/ISO_639
 * @link http://en.wikipedia.org/wiki/ISO_3166-1
 * @link http://loc.gov/standards/iso639-2/php/code_list.php
 *
 * @package Titon\G11n
 * @events
 *      g11n.onInit(G11n $g11n, $code)
 *      g11n.onUse(G11n $g11n, Locale $locale)
 *      g11n.onCascade(G11n $g11n, array $cycle)
 */
class G11n implements Subject {
    use EmitsEvents;

    /**
     * Default configuration.
     *
     * @var array {
     *      @var bool $prependUrl  Prepend the locale onto routes
     *      @var bool $storeCookie Store the current locale in a cookie
     * }
     */
    protected $config = [
        'prependUrl' => true,
        'storeCookie' => false
    ];

    /**
     * Currently active locale based on the client.
     *
     * @var \Titon\G11n\Locale
     */
    protected ?Locale $current;

    /**
     * Fallback locale if none can be found.
     *
     * @var \Titon\G11n\Locale
     */
    protected ?Locale $fallback;

    /**
     * Supported list of locales.
     *
     * @var \Titon\G11n\LocaleMap
     */
    protected LocaleMap $locales = Map {};

    /**
     * Resource lookup paths.
     *
     * @var \Titon\Io\DomainPathMap
     */
    protected DomainPathMap $paths = Map {};

    /**
     * Translator used for string fetching and parsing.
     *
     * @var \Titon\G11n\Translator
     */
    protected Translator $translator;

    /**
     * Store the translator.
     *
     * @param \Titon\G11n\Translator $translator
     */
    public function __construct(Translator $translator) {
        $this->translator = $translator;
    }

    /**
     * Sets up the application with the defined locale key; the key will be formatted to a lowercase dashed URL friendly format.
     * The system will then attempt to load the locale resource bundle and finalize configuration settings.
     *
     * @param \Titon\G11n\Locale $locale
     * @return \Titon\G11n\Locale
     */
    public function addLocale(Locale $locale): Locale {
        $key = static::canonicalize($locale->getCode());

        if ($this->locales->contains($key)) {
            return $this->locales[$key];
        }

        // Configure and initialize
        foreach ($this->getResourcePaths() as $domain => $paths) {
            $locale->addResourcePaths($domain, $paths);
        }

        $locale->initialize();

        // Set the locale
        $this->locales[$key] = $locale;

        // Set the parent as well
        if ($parent = $locale->getParentLocale()) {
            $this->addLocale($parent);
        }

        // Set fallback if none defined
        if (!$this->fallback) {
            $this->setFallback($key);
        }

        return $locale;
    }

    /**
     * Add multiple resource path lookups.
     *
     * @param string $domain
     * @param \Titon\Io\PathList $paths
     * @return $this
     */
    public function addResourcePaths(string $domain, PathList $paths): this {
        foreach ($paths as $path) {
            $this->paths[$domain][] = $path;
        }

        return $this;
    }

    /**
     * Get a list of locales and fallback locales in descending order starting from the current locale.
     *
     * @return Vector<string>
     */
    <<__Memoize>>
    public function cascade(): Vector<string> {
        $cycle = [];

        foreach ([$this->current(), $this->getFallback()] as $locale) {
            while ($locale instanceof Locale) {
                $cycle[] = $locale->getCode();

                $locale = $locale->getParentLocale();
            }
        }

        $cycle = array_unique($cycle);

        $this->emit('g11n.onCascade', [$this, &$cycle]);

        return new Vector($cycle);
    }

    /**
     * Return the current locale.
     *
     * @return \Titon\G11n\Locale
     */
    public function current(): ?Locale {
        return $this->current;
    }

    /**
     * Return the fallback locale.
     *
     * @return \Titon\G11n\Locale
     */
    public function getFallback(): ?Locale {
        return $this->fallback;
    }

    /**
     * Returns a list of supported locales.
     *
     * @return \Titon\G11n\LocaleMap
     */
    public function getLocales(): LocaleMap {
        return $this->locales;
    }

    /**
     * Return all resource paths indexed by domain.
     *
     * @return \Titon\Io\DomainPathMap
     */
    public function getResourcePaths(): DomainPathMap {
        return $this->paths;
    }

    /**
     * Return the translator.
     *
     * @return \Titon\G11n\Translator
     */
    public function getTranslator(): Translator {
        return $this->translator;
    }

    /**
     * Detect which locale to use based on the clients Accept-Language header.
     *
     * @throws \Titon\G11n\Exception\MissingTranslatorException
     */
    public function initialize() {
        if (!$this->isEnabled()) {
            return;
        }

        $current = null;

        // Determine via cookie
        if (!empty($_COOKIE['locale']) && isset($this->locales[$_COOKIE['locale']])) {
            $current = $_COOKIE['locale'];

        // Determine locale based on HTTP headers
        } else if (isset($_SERVER['HTTP_ACCEPT_LANGUAGE'])) {
            $header = mb_strtolower($_SERVER['HTTP_ACCEPT_LANGUAGE']);

            if (mb_strpos($header, ';') !== false) {
                $header = mb_strstr($header, ';', true);
            }

            $header = explode(',', $header);

            if (count($header) > 0) {
                foreach ($header as $key) {
                    if (isset($this->locales[$key])) {
                        $current = $key;
                        break;
                    }
                }
            }
        }

        // Set current to the fallback if none found
        if ($current === null) {
            $current = $this->fallback->getCode();
        }

        // Check for a translator
        if (!$this->translator) {
            throw new MissingTranslatorException('A translator is required for G11n message parsing');
        }

        $this->emit('g11n.onInit', [$this, &$current]);

        // Apply the locale
        $this->useLocale($current);
    }

    /**
     * Does the current locale matched the passed key?
     *
     * @param string $key
     * @return bool
     */
    public function is(string $key): bool {
        $code = $this->current()?->getCode();

        return ($code === $key || $this->canonicalize($code) === $key);
    }

    /**
     * G11n will be enabled if more than 1 locale has been setup.
     *
     * @return bool
     */
    public function isEnabled(): bool {
        return (count($this->locales) > 0);
    }

    /**
     * Register events for this package.
     *
     * @return array
     */
    public function registerEvents() {
        if (!$this->isEnabled() || !$this->getConfig('prependUrl')) {
            return [];
        }

        return [
            'route.preMatch' => 'resolveRoute'
        ];
    }

    /**
     * When the Router initializes, check for the existence of a locale in the URL.
     * If the locale exists, verify it. If either of these fail, redirect with the fallback locale.
     * This event must be bound to the Router to work.
     *
     * @param \Titon\Event\Event $event
     * @param \Titon\Route\Router $router
     * @param string $url
     */
    public function resolveRoute(Event $event, Router $router, $url) {
        if (PHP_SAPI === 'cli') {
            return;
        }

        $locales = $this->getLocales();
        $redirect = '/' . $this->getFallback()->getCode();
        $base = $router->base();

        // Double leading slashes redirect outside of host
        if ($base !== '/') {
            $redirect = $base . $redirect;
        }

        // Path doesn't start with a locale
        if (!preg_match('/^\/' . LocaleRoute::LOCALE . '\/(.*)?/', $url, $matches)) {

            // Check for locales that don't pass because of no ending slash
            if (empty($locales[trim($url, '/')])) {
                header('Location: ' . $redirect . $url);
                exit();
            }

        // Or if that locale is not supported
        } else if (empty($locales[$matches[1]])) {
            header('Location: ' . $redirect . $matches[2]);
            exit();
        }
    }

    /**
     * Define the fallback locale to use if none can be found or is not supported.
     *
     * @uses Titon\Common\Config
     *
     * @param string $key
     * @return $this
     * @throws \Titon\G11n\Exception\MissingLocaleException
     */
    public function setFallback(string $key): this {
        $key = $this->canonicalize($key);

        if (!isset($this->locales[$key])) {
            throw new MissingLocaleException(sprintf('Locale %s has not been setup', $key));
        }

        $this->fallback = $this->locales[$key];

        Config::set('titon.locale.fallback', $key);

        return $this;
    }

    /**
     * Sets the translator to use in the string locating and translating process.
     *
     * @param \Titon\G11n\Translator $translator
     * @return \Titon\G11n\Translator
     */
    public function setTranslator(Translator $translator): this {
        $this->translator = $translator;

        return $translator;
    }

    /**
     * Return a translated string using the translator.
     * If a storage engine is present, read and write from the cache.
     *
     * @param string $key
     * @param array $params
     * @return string
     */
    public function translate($key, array $params = []) {
        return $this->getTranslator()->translate($key, $params);
    }

    /**
     * Set the locale using PHPs built in setlocale().
     *
     * @link http://php.net/setlocale
     * @link http://php.net/manual/locale.setdefault.php
     *
     * @uses Titon\Common\Config
     *
     * @param string $key
     * @return \Titon\G11n\Locale
     * @throws \Titon\G11n\Exception\MissingLocaleException
     */
    public function useLocale($key) {
        $key = self::canonicalize($key);

        if (!isset($this->locales[$key])) {
            throw new MissingLocaleException(sprintf('Locale %s does not exist', $key));
        }

        $locale = $this->locales[$key];
        $locales = [$locale];
        $options = [];

        if ($this->getFallback()->getCode() != $locale->getCode()) {
            $locales[] = $this->getFallback();
        }

        /** @var \Titon\G11n\Locale $loc */
        foreach ($locales as $loc) {
            $config = $loc->allConfig();

            $options[] = $config['code'] . '.UTF8';
            $options[] = $config['code'] . '.UTF-8';
            $options[] = $config['code'];

            if (!empty($config['iso3'])) {
                foreach ((array) $config['iso3'] as $iso3) {
                    $options[] = $iso3 . '.UTF8';
                    $options[] = $iso3 . '.UTF-8';
                    $options[] = $iso3;
                }
            }

            if (!empty($config['iso2'])) {
                $options[] = $config['iso2'] . '.UTF8';
                $options[] = $config['iso2'] . '.UTF-8';
                $options[] = $config['iso2'];
            }
        }

        // Set environment
        $code = $locale->getCode();

        putenv('LC_ALL=' . $code);
        setlocale(LC_ALL, $options);

        \Locale::setDefault($code);

        $this->current = $locale;

        Config::set('titon.locale.current', $code);
        Config::set('titon.locale.cascade', $this->cascade());

        $this->emit('g11n.onUse', [$this, $locale]);

        // Store via cookie
        if ($this->getConfig('storeCookie')) {
            $_COOKIE['locale'] = $key;
        }

        return $locale;
    }

}
