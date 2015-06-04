<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Intl;

use Titon\Event\Event;
use Titon\Event\Listener;
use Titon\Event\ListenerMap;
use Titon\Route\Event\MatchingEvent;
use Titon\Route\LocaleRoute;
use Titon\Route\UrlBuilder;

/**
 * The `RouteResolver` is used for verifying that URLs matched during the router process contain a locale.
 * If a locale does not exist, or is not supported, the page is redirected to the URL prepended with a fallback locale.
 *
 * @package Titon\Intl
 */
class RouteResolver implements Listener {

    /**
     * Translator instance.
     *
     * @var \Titon\Intl\Translator
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
     * @param \Titon\Intl\Translator $translator
     * @param \Titon\Route\UrlBuilder $urlBuilder
     */
    public function __construct(Translator $translator, UrlBuilder $urlBuilder) {
        $this->translator = $translator;
        $this->urlBuilder = $urlBuilder;
    }

    /**
     * Return the translator.
     *
     * @return \Titon\Intl\Translator
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
     * Resolve the current URL against the `Router`s matching event.
     *
     * @param \Titon\Event\Event $event
     */
    public function onResolve(Event $event): void {
        if (!$this->getTranslator()->isEnabled()) {
            return;
        }

        invariant($event instanceof MatchingEvent, 'Must be a MatchingEvent.');

        $this->resolve($event->getUrl());
    }

    /**
     * Perform a redirect.
     *
     * @param string $redirectTo
     */
    public function redirect(string $redirectTo): void {
        header('Location: ' . $redirectTo);
        exit();
    }

    /**
     * When the router attempts to match, check for the existence of a locale in the URL.
     * If the locale exists, verify it. If either of these fail, redirect with the fallback locale.
     * This event must be bound to the router to work.
     *
     * @param string $url
     */
    public function resolve(string $url): void {
        $translator = $this->getTranslator();
        $redirectTo = $this->getUrlBuilder()->getBase() . $translator->getFallback()->getCode();
        $locales = $translator->getLocales();
        $matches = [];

        // Path doesn't start with a locale
        if (!preg_match('/^\/([a-z]{2}(?:-[a-z]{2})?)\/(.*?)$/', $url, $matches)) {
            $possibleLocale = trim($url, '/');
            $canonicalLocale = Locale::canonicalize($possibleLocale);

            // URL is in locale format but is not a supported locale
            if (preg_match('/^' . LocaleRoute::LOCALE . '$/', $possibleLocale) && !$locales->contains($canonicalLocale)) {
                $this->redirect($redirectTo);

            // If not in locale format, persist the path
            } else if (!$locales->contains($canonicalLocale)) {
                $this->redirect($redirectTo . $url);
            }

        // Path does start with a locale but the locale is not supported
        } else if (!$locales->contains(Locale::canonicalize($matches[1]))) {
            $this->redirect($redirectTo . '/' . $matches[2]);
        }
    }

    /**
     * Register events.
     *
     * @return \Titon\Event\ListenerMap
     */
    public function subscribeToEvents(): ListenerMap {
        return Map {'route.matching' => 'resolve'};
    }

}
