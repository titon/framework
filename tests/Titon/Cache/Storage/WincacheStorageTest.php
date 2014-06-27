<?php
namespace Titon\Cache\Storage;

class WincacheStorageTest extends AbstractStorageTest {

    protected function setUp() {
        if (!extension_loaded('wincache')) {
            $this->markTestSkipped('Wincache is not installed or configured properly');
        }

        $this->object = new WincacheStorage();

        parent::setUp();
    }

}