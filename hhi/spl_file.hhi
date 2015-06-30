<?hh // decl /* -*- php -*- */
/**
 * Copyright (c) 2014, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the 'hack' directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 *
 */

class FilesystemIterator extends DirectoryIterator {
    const CURRENT_MODE_MASK = 240;
    const CURRENT_AS_PATHNAME = 32;
    const CURRENT_AS_FILEINFO = 0;
    const CURRENT_AS_SELF = 16;
    const KEY_MODE_MASK = 3840;
    const KEY_AS_PATHNAME = 0;
    const FOLLOW_SYMLINKS = 512;
    const KEY_AS_FILENAME = 256;
    const NEW_CURRENT_AND_KEY = 256;
    const SKIP_DOTS = 4096;
    const UNIX_PATHS = 8192;
    public function __construct($path, $flags);
    public function getFlags();
    public function setFlags($flags = null);
}

class RecursiveDirectoryIterator<Tv> extends FilesystemIterator implements RecursiveIterator<Tv> {
    public function __construct($path, $flags);
    public function getChildren(): this;
    public function hasChildren(): bool;
    public function getSubPath();
    public function getSubPathname();
}

class GlobIterator extends FilesystemIterator implements Countable {
    public function __construct($path, $flags);
    public function count();
}

class SplStack<T> extends SplDoublyLinkedList<T> {
    public function setIteratorMode($mode);
}

abstract class SplHeap<T> implements Iterator<T>, Countable {
    public function extract();
    public function insert($value);
    public function top();
    public function isEmpty();
    public function recoverFromCorruption();
    public function current();
    public function next();
    public function key();
    public function valid();
    public function rewind();
    public function count();
    abstract protected function compare($value1, $value2);
}

class SplMinHeap<T> extends SplHeap<T> {
    protected function compare($value1, $value2);
}

class SplMaxHeap<T> extends SplHeap<T> {
    protected function compare($value1, $value2);
}

class SplPriorityQueue<T> implements Iterator<T>, Countable {
    const EXTR_BOTH = 3;
    const EXTR_PRIORITY = 2;
    const EXTR_DATA = 1;
    public function compare($priority1, $priority2);
    public function insert($value, $priority);
    public function setExtractFlags($flags);
    public function top();
    public function extract();
    public function isEmpty();
    public function recoverFromCorruption();
    public function current();
    public function next();
    public function key();
    public function valid();
    public function rewind();
    public function count();
}

class SplFixedArray<Tk, Tv> implements Iterator<Tv>, ArrayAccess<Tk, Tv>, Countable {
    public function __construct($size = 0);
    public function toArray();
    public static function fromArray(array $array, $save_indexes = true);
    public function getSize();
    public function setSize($size);
    public function current();
    public function next();
    public function key();
    public function valid();
    public function rewind();
    public function count();
    public function offsetExists($offset);
    public function offsetGet($offset);
    public function offsetSet($offset, $value);
    public function offsetUnset($offset);
}

interface SplObserver {
    public function update(SplSubject $subject);
}

interface SplSubject {
    public function attach(SplObserver $observer);
    public function detach(SplObserver $observer);
    public function notify();
}

class SplObjectStorage<Tk, Tv> implements Countable, Iterator<Tv>, Serializable, ArrayAccess<Tk, Tv> {
    public function attach($object, $data = null);
    public function detach($object);
    public function contains($object);
    public function addAll(SplObjectStorage $storage);
    public function removeAll(SplObjectStorage $storage);
    public function removeAllExcept(SplObjectStorage $storage);
    public function getInfo();
    public function setInfo($data);
    public function getHash($object);
    public function current();
    public function next();
    public function key();
    public function valid();
    public function rewind();
    public function count();
    public function offsetExists($offset);
    public function offsetGet($offset);
    public function offsetSet($offset, $value);
    public function offsetUnset($offset);
    public function serialize();
    public function unserialize($serialized);
}

class MultipleIterator<T> implements Iterator<T> {
    const MIT_NEED_ANY = 0;
    const MIT_NEED_ALL = 1;
    const MIT_KEYS_NUMERIC = 0;
    const MIT_KEYS_ASSOC = 2;
    public function __construct($flags);
    public function getFlags();
    public function setFlags($flags);
    public function attachIterator(Iterator $iterator, $infos = null);
    public function detachIterator(Iterator $iterator);
    public function containsIterator(Iterator $iterator);
    public function countIterators();
    public function current();
    public function next();
    public function key();
    public function valid();
    public function rewind();
    public function count();
}

class SplType {
    const __default = null;
    public function __construct($initial_value, $strict);
}

class SplInt extends SplType {
    const __default = 0;
}

class SplFloat extends SplType {
    const __default = 0;
}

class SplString extends SplType {
    const __default = 0;
}

class SplEnum extends SplType {
    const __default = null;
    public function __construct($initial_value, $strict);
    public function getConstList($include_default = false);
}

class SplBool extends SplEnum {
    const __default = false;
    const true = true;
    const false = false;
}
