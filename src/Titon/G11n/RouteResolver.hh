<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\G11n;

use Titon\Event\Event;
use Titon\Event\Listener;
use Titon\Event\ListenerMap;
use Titon\Route\Event\MatchingEvent;
use Titon\Route\UrlBuilder;

/**
 * The `RouteResolver` is used for verifying that URLs matched during the router process contain a locale.
 * If a locale does not exist, or is not supported, the page is redirected to the URL prepended with a fallback locale.
 *
 * @package Titon\G11n
 */
class RouteResolver implements Listener {

    /**
     * Translator instance.
     *
     * @var \Titon\G11n\Translator
     */
    protected Translator $translator;

    /**
     * URL builder instance.
     *
     * @var \Titon\Route\UrlBuilder
     */
    protected UrlBuilder $urlBuilder;

    /**
     * Store the translator.
     *
     * @param \Titon\G11n\Translator $translator
     * @param \Titon\Route\UrlBuilder $urlBuilder
     */
    public function __construct(Translator $translator, UrlBuilder $urlBuilder) {
        $this->translator = $translator;
        $this->urlBuilder = $urlBuilder;
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
     * Return the URL builder.
     *
     * @return \Titon\Route\UrlBuilder
     */
    public function getUrlBuilder(): UrlBuilder {
        return $this->urlBuilder;
    }

    /**
     * Register events.
     *
     * @return \Titon\Event\ListenerMap
     */
    public function subscribeToEvents(): ListenerMap;
        return Map {'route.matching' => 'resolve'};
    }

    /**
     * When the router attempts to match, check for the existence of a locale in the URL.
     * If the locale exists, verify it. If either of these fail, redirect with the fallback locale.
     * This event must be bound to the router to work.
     *
     * @param \Titon\Event\Event $event
     */
    public function resolve(Event $event) {
        invariant($event instanceof MatchingEvent, 'Must be a MatchingEvent.');

        $translator = $this->getTranslator();

        if (PHP_SAPI === 'cli' || !$translator->isEnabled()) {
            return;
        }

        $url = $event->getUrl();
        $redirectTo = $this->getUrlBuilder()->getBase() . $translator->getFallback()->getCode();
        $locales = $translator->getLocales();
        $matches = [];

        // Path doesn't start with a locale
        if (!preg_match('/^\/' . LocaleRoute::LOCALE . '/', $url, $matches)) {

            // Check for locales that don't pass because of no ending slash
            if (!$locales->contains(trim($url, '/'))) {
                header('Location: ' . $redirectTo . $url);
                exit();
            }

        // Or if that locale is not supported
        } else if (!$locales->contains($locales[$matches[1]])) {
            header('Location: ' . $redirectTo . $matches[2]);
            exit();
        }
    }

}
