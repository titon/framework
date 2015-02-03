<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Event;

/**
 * The Listener interface defines groups of observers to subscribe for events.
 *
 * @package Titon\Event
 */
interface Listener {

    /**
     * Return an map of observers to subscribe to events.
     * The collection key should be the name of the event, while the value should be the callable.
     *
     * @return \Titon\Event\ListenerMap
     */
    public function subscribeToEvents(): ListenerMap;

}
