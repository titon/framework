<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Intl\Utility;

use Titon\Intl\Bag\FormatBag;
use Titon\Intl\MessageLoader;
use Titon\Utility\Col;
use Titon\Utility\OptionMap;
use Titon\Utility\TimeMessageMap;

/**
 * Enhance the parent Format class by providing localized formatting rule support.
 *
 * @package Titon\Intl\Utility
 */
class Format extends \Titon\Utility\Format {

    /**
     * {@inheritdoc}
     */
    public static function date(mixed $time, string $format = ''): string {
        $patterns = static::loadFormatPatterns();

        if ($patterns !== null && !$format) {
            $format = $patterns->getDate();
        }

        if ($format) {
            return parent::date($time, $format);
        }

        return parent::date($time);
    }

    /**
     * {@inheritdoc}
     */
    public static function datetime(mixed $time, string $format = ''): string {
        $patterns = static::loadFormatPatterns();

        if ($patterns !== null && !$format) {
            $format = $patterns->getDatetime();
        }

        if ($format) {
            return parent::datetime($time, $format);
        }

        return parent::datetime($time);
    }

    /**
     * Load and return a `FormatBag` from the currently detected locale.
     * If no locale is found, or the translator is not enabled, return null.
     *
     * @return \Titon\Intl\Bag\FormatBag
     */
    public static function loadFormatPatterns(): ?FormatBag {
        $translator = translator_context();

        if (!$translator->isEnabled()) {
            return null;
        }

        return $translator->current()?->getFormatPatterns();
    }

    /**
     * {@inheritdoc}
     */
    public static function phone(string $value, mixed $format = ''): string {
        $patterns = static::loadFormatPatterns();

        if ($patterns !== null) {
            if ($map = $patterns->getPhone()) {
                if ($format instanceof Map) {
                    $format = Col::merge($map, $format);
                } else {
                    $format = $map;
                }
            }
        }

        return parent::phone($value, $format);
    }

    /**
     * {@inheritdoc}
     */
    public static function relativeTime(mixed $time, OptionMap $options = Map {}, TimeMessageMap $messages = Map {}): string {
        $translator = translator_context();

        if ($translator->isEnabled() && $translator->current() !== null) {
            $catalog = $translator->getMessageLoader()->loadCatalog(MessageLoader::DEFAULT_DOMAIN, 'format');
            $formatMessages = $catalog->getMessages();

            // Find option messages
            $optionKeys = Vector {'now', 'in', 'ago'};

            foreach ($optionKeys as $optionKey) {
                if (!$options->contains($optionKey) && $formatMessages->contains('time.relative.' . $optionKey)) {
                    $options[$optionKey] = $formatMessages['time.relative.' . $optionKey];
                }
            }

            // Find messages
            $messageKeys = Map {
                'seconds'   => Vector {'second', 'second', 'seconds'},
                'minutes'   => Vector {'minute', 'minute', 'minutes'},
                'hours'     => Vector {'hour', 'hour', 'hours'},
                'days'      => Vector {'day', 'day', 'days'},
                'weeks'     => Vector {'week', 'week', 'weeks'},
                'months'    => Vector {'month', 'month', 'months'},
                'years'     => Vector {'year', 'year', 'years'}
            };

            foreach ($messageKeys as $parentKey => $childKeys) {
                foreach ($childKeys as $i => $childKey) {
                    $messageKey = 'time.relative.' . $childKey;

                    // First key is shorthand
                    if ($i === 0) {
                        $messageKey .= '.short';
                    }

                    // Don't overwrite custom messages
                    if ($messages->contains($parentKey) && $messages[$parentKey]->containsKey($i)) {
                        continue;
                    }

                    if ($formatMessages->contains($messageKey)) {
                        if (!$messages->contains($parentKey)) {
                            $messages[$parentKey] = Map {};
                        }

                        $messages[$parentKey][$i] = $formatMessages[$messageKey];
                    }
                }
            }
        }

        return parent::relativeTime($time, $options, $messages);
    }

    /**
     * {@inheritdoc}
     */
    public static function ssn(string $value, string $format = ''): string {
        $patterns = static::loadFormatPatterns();

        if ($patterns !== null && !$format) {
            $format = $patterns->getSSN();
        }

        return parent::ssn($value, $format);
    }

    /**
     * {@inheritdoc}
     */
    public static function time(mixed $time, string $format = ''): string {
        $patterns = static::loadFormatPatterns();

        if ($patterns !== null && !$format) {
            $format = $patterns->getTime();
        }

        if ($format) {
            return parent::time($time, $format);
        }

        return parent::time($time);
    }

}
