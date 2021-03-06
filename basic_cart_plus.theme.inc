<?php
/**
 * @file
 * Basic Cart Plus main theme function wrapper.
 */

module_load_include('inc', 'basic_cart_plus', 'basic_cart_plus.cart');

/**
 * Theme function implementation.
 */
function basic_cart_plus_price($vars)
{
    $price = $vars['price'];
    $html  = '<div class="basic-cart-price"><span class="basic-cart-price-label">' . t('Price') . ': </span><strong>' . $price . '</strong></div>';
    return $html;
}

/**
 * Theme function implementation.
 */
function basic_cart_plus_add_to_cart_button($vars)
{
    $nid         = $vars['nid'];
    $button_text = $vars['add_to_cart_title'];
    return '<div class="basic-cart-add-to-cart">
            ' . l(t($button_text), 'cart/add/' . $nid, array(
        'attributes' => array(
            'class' => array(
                'button btn basic-cart-add-to-cart-link'
            )
        )
    )) . '
          </div>';
}

/**
 * Function returns an array to render add to cart link
 */
function basic_cart_plus_add_to_cart_link($nid)
{
    $link = array(
        '#type' => 'link',
        '#id' => drupal_html_id('basic-cart-add-to-cart-link-' . $nid),
        '#prefix' => '<div class="basic-cart-add-to-cart">',
        '#suffix' => '</div>',
        '#title' => t('Add to cart'),
        '#href' => 'cart/add/' . $nid,
        '#attributes' => array(
            'class' => array(
                'button',
                'basic-cart-add-to-cart-link'
            )
        )
    );

    return $link;
}

/**
 * Theme implementation for rendering the total price.
 *
 * @todo find a way to pass $price through the variables array.
 */
function theme_basic_cart_plus_cart_total_price($variables)
{
    $price = basic_cart_plus_get_total_price();
    $total = basic_cart_plus_price_format($price->total);

    // Building the HTML.
    $html = '<div class="basic-cart-cart-total-price-contents row">';
    $html .= '  <div class="basic-cart-total-price cell">' . t('Total price') . ': <strong>' . $total . '</strong></div>';
    $html .= '</div>';

    $vat_is_enabled = (int) config_get('basic_cart_plus.settings', 'basic_cart_plus_vat_state');

    if (!empty($vat_is_enabled) && $vat_is_enabled) {
        $vat_value = basic_cart_plus_price_format($price->vat);
        $html .= '<div class="basic-cart-cart-total-vat-contents row">';
        $html .= '  <div class="basic-cart-total-vat cell">' . t('Total VAT') . ': <strong>' . $vat_value . '</strong></div>';
        $html .= '</div>';
    }

    return $html;
}

/**
 * Theme implementation for rendering the cart element.
 */
function theme_basic_cart_plus_render_cart_element($variables)
{
    // Element name and nid.
    $name = $variables['form']['#name'];
    $nid  = (int) str_replace(array(
        'cartcontents[',
        ']'
    ), '', $name);
    if (empty($nid)) {
        return '';
    }

    // Getting the node for it's title and description.
    // $node = basic_cart_braintree_get_cart($nid);
    $node        = node_load($nid);
    // Node description.
    $delete_link = "<a href='cart/remove/" . $node->nid . "' title='Remove product from cart'><img alt='Remove product from cart' class='basic-cart-delete-image-image' src='" . base_path() . backdrop_get_path('module', 'basic_cart_plus') . "/images/delete2.png'></a>";
    $desc        = '';
    if (isset($node->basic_cart_plus_node_description)) {
        $desc = drupal_strlen($node->basic_cart_plus_node_description) > 50 ? truncate_utf8($node->basic_cart_plus_node_description, 50) : $node->basic_cart_plus_node_description;
    }
    // Price and currency.
    $unit_price = isset($node->price["und"][0]["value"]) ? $node->price["und"][0]["value"] : 0;
    $unit_price = basic_cart_plus_price_format($unit_price);

    // Prefix.
    $prefix = '<div class="basic-cart-cart-contents row">';
    $prefix .= '  <div class="basic-cart-delete-image cell">' . $delete_link . '</div>';


    $member_info = basic_cart_plus_node_membership_info($node);
    if (($member_info["is_member_node"] > 0 && $member_info["is_member_user"] == 0) || ($member_info["is_bronze_node"] > 0 && $member_info["is_bronze_user"] == 0) || ($member_info["is_silver_node"] > 0 && $member_info["is_silver_user"] == 0) || ($member_info["is_gold_node"] > 0 && $member_info["is_gold_user"] == 0)) {
        $sitemembership = "yes";
        $prefix .= '<div class="basic-cart-cart-node-title cell">' . l("Site Membership", 'node/' . $node->nid) . '</div>';
    }

    if (!isset($sitemembership)) {
        $prefix .= '  <div class="basic-cart-cart-node-title cell">' . l($node->title, 'node/' . $nid) . '<br />';
    }

    $prefix .= '    <span class="basic-cart-cart-node-summary">' . $desc . '</span>';
    $prefix .= '  </div>';
    $prefix .= '  <div class="cell basic-cart-cart-unit-price"><strong>' . $unit_price . '</strong></div>';
    $prefix .= '  <div class="cell basic-cart-cart-x">x</div>';
    $prefix .= '  <div class="basic-cart-cart-quantity cell">';
    $prefix .= '    <div class="cell">';
    // Suffix.
    $suffix = '    </div>';
    $suffix .= '  </div>';
    $suffix .= '</div>';

    // Rendering the element as textfield.
    $quantity = theme('textfield', $variables['form']);
    // Full view return.
    return $prefix . $quantity . $suffix;
}
