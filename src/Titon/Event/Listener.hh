<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Event;

type ListenerMap = Map<string, mixed>;

/**
 * Interface the defines groups of event callbacks to register for a class.
 *
 * @package Titon\Event
 */
interface Listener {

    /**
     * Return an array of callbacks to register to events.
     * The array key should be the name of the event, while the value should be the callback.
     * The callback could be an array containing the method and priority, or a string of the method.
     *
     * @return \Titon\Event\ListenerMap
     */
    public function registerEvents(): ListenerMap;

}