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

class RecursiveIteratorIterator<T> implements Iterator<T>, OuterIterator<T> {
    const LEAVES_ONLY = 0;
    const SELF_FIRST = 1;
    const CHILD_FIRST = 2;
    const CATCH_GET_CHILD = 16;
    public function __construct(Traversable<T> $iterator, $mode, $flags = null);
    public function rewind();
    public function valid();
    public function key();
    public function current();
    public function next();
    public function getDepth();
    public function getSubIterator($level);
    public function getInnerIterator();
    public function beginIteration();
    public function endIteration();
    public function callHasChildren();
    public function callGetChildren();
    public function beginChildren();
    public function endChildren();
    public function nextElement();
    public function setMaxDepth($max_depth = -1);
    public function getMaxDepth();
}
