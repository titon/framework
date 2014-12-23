<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Route\Mixin;

type FilterList = Vector<string>;

/**
 * Provides functionality for filters.
 *
 * @package Titon\Route\Mixin
 */
trait FilterMixin {

    /**
     * Filters to trigger once the route has been matched.
     *
     * @var \Titon\Route\Mixin\FilterList
     */
    protected FilterList $_filters = Vector {};

    /**
     * Add a filter by name.
     *
     * @param string $filter
     * @return $this
     */
    public function addFilter(string $filter): this {
        if (!in_array($filter, $this->_filters)) {
            $this->_filters[] = $filter;
        }

        return $this;
    }

    /**
     * Add multiple filters by name.
     *
     * @param \Titon\Route\Mixin\FilterList $filters
     * @return $this
     */
    public function addFilters(FilterList $filters): this {
        foreach ($filters as $filter) {
            $this->addFilter($filter);
        }

        return $this;
    }

    /**
     * Return all filters.
     *
     * @return \Titon\Route\Mixin\FilterList
     */
    public function getFilters(): FilterList {
        return $this->_filters;
    }

    /**
     * Set the list of filters to process.
     *
     * @param \Titon\Route\Mixin\FilterList $filters
     * @return $this
     */
    public function setFilters(FilterList $filters): this {
        $this->_filters = $filters;

        return $this;
    }

}