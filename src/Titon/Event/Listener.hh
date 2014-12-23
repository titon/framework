<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Event;

type ListenerMap = Map<string, mixed>;
type ListenerOption = shape('method' => string, 'priority' => int, 'once' => bool);

/**
 * The Listener interface defines groups of observers to register for events.
 *
 * @package Titon\Event
 */
interface Listener {

    /**
     * Return an array of observers to register to events.
     * The collection key should be the name of the event, while the value should be the callable.
     *
     * @return \Titon\Event\ListenerMap
     */
    public function registerEvents(): ListenerMap;

}