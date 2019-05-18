/****************************************************************************
Copyright (c) 2019 Cognitics, Inc.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
****************************************************************************/
// 2013-09-18 Aaron Brinton <abrinton@cognitics.net>
#pragma once

#include <string>
#include <iostream>

namespace cognitics
{
    namespace uri
    {
        //--------------------------------------------------------------------------------
        // traits / implement these for your uri type

        template <typename T>
        struct traits
        {
            //static std::string get_uri(const T &uri);
            //static std::string get_scheme(const T &uri);
            //static std::string get_content(const T &uri);
            //static std::string get_hierarchy(const T &uri);
            //static std::string get_query(const T &uri);
            //static std::string get_fragment(const T &uri);
            //static std::string get_authority(const T &uri);
            //static std::string get_path(const T &uri);
            //static std::string get_username(const T &url);
            //static std::string get_password(const T &url);
            //static std::string get_host(const T &url);
            //static std::string get_port(const T &url);
            //static std::string get_nid(const T &urn);
            //static std::string get_nss(const T &urn);
            //static bool set_uri(T &uri, const std::string &value);
            //static bool set_scheme(T &uri, const std::string &value);
            //static bool set_content(T &uri, const std::string &value);
            //static bool set_hierarchy(T &uri, const std::string &value);
            //static bool set_query(T &uri, const std::string &value);
            //static bool set_fragment(T &uri, const std::string &value);
            //static bool set_authority(T &uri, const std::string &value);
            //static bool set_path(T &uri, const std::string &value);
            //static bool set_username(T &url, const std::string &value);
            //static bool set_password(T &url, const std::string &value);
            //static bool set_host(T &url, const std::string &value);
            //static bool set_port(T &url, const std::string &value);
            //static bool set_nid(T &urn, const std::string &value);
            //static bool set_nss(T &urn, const std::string &value);
            //static bool equal(const T &uri1, const T &uri2);
            //static bool less(const T &uri1, const T &uri2);
        };

        namespace content
        {
            template <typename T>
            struct traits
            {
                //static std::string get_hierarchy(const T &content);
                //static std::string get_query(const T &content);
                //static std::string get_fragment(const T &content);
                //static bool set_hierarchy(T &content, const std::string &value);
                //static bool set_query(T &content, const std::string &value);
                //static bool set_fragment(T &content, const std::string &value);
            };
        }

        namespace hierarchy
        {
            template <typename T>
            struct traits
            {
                //static std::string get_authority(const T &hierarchy);
                //static std::string get_path(const T &hierarchy);
                //static bool set_authority(T &hierarchy, const std::string &value);
                //static bool set_path(T &hierarchy, const std::string &value);
            };
        }

        namespace url
        {
            template <typename T>
            struct traits
            {
                //static std::string get_username(const T &authority);
                //static std::string get_password(const T &authority);
                //static std::string get_host(const T &authority);
                //static std::string get_port(const T &authority);
                //static bool set_username(T &authority, const std::string &value);
                //static bool set_password(T &authority, const std::string &value);
                //static bool set_host(T &authority, const std::string &value);
                //static bool set_port(T &authority, const std::string &value);
            };

        }

        namespace urn
        {
            template <typename T>
            struct traits
            {
                //static std::string get_nid(const T &content);
                //static std::string get_nss(const T &content);
                //static bool set_nid(T &content, const std::string &value);
                //static bool set_nss(T &content, const std::string &value);
            };
        }

        //--------------------------------------------------------------------------------
        // inline trait wrapper functions

        template <typename T> inline std::string get_uri(const T &uri) { return traits<T>::get_uri(uri); }
        template <typename T> inline std::string get_scheme(const T &uri) { return traits<T>::get_scheme(uri); }
        template <typename T> inline std::string get_content(const T &uri) { return traits<T>::get_content(uri); }
        template <typename T> inline std::string get_hierarchy(const T &uri) { return traits<T>::get_hierarchy(uri); }
        template <typename T> inline std::string get_query(const T &uri) { return traits<T>::get_query(uri); }
        template <typename T> inline std::string get_fragment(const T &uri) { return traits<T>::get_fragment(uri); }
        template <typename T> inline std::string get_authority(const T &uri) { return traits<T>::get_authority(uri); }
        template <typename T> inline std::string get_path(const T &uri) { return traits<T>::get_path(uri); }
        template <typename T> inline std::string get_username(const T &url) { return traits<T>::get_username(url); }
        template <typename T> inline std::string get_password(const T &url) { return traits<T>::get_password(url); }
        template <typename T> inline std::string get_host(const T &url) { return traits<T>::get_host(url); }
        template <typename T> inline std::string get_port(const T &url) { return traits<T>::get_port(url); }
        template <typename T> inline std::string get_nid(const T &urn) { return traits<T>::get_nid(urn); }
        template <typename T> inline std::string get_nss(const T &urn) { return traits<T>::get_nss(urn); }
        template <typename T> inline bool set_uri(T &uri, const std::string &value) { return traits<T>::set_uri(uri, value); }
        template <typename T> inline bool set_scheme(T &uri, const std::string &value) { return traits<T>::set_scheme(uri, value); }
        template <typename T> inline bool set_content(T &uri, const std::string &value) { return traits<T>::set_content(uri, value); }
        template <typename T> inline bool set_hierarchy(T &uri, const std::string &value) { return traits<T>::set_hierarchy(uri, value); }
        template <typename T> inline bool set_query(T &uri, const std::string &value) { return traits<T>::set_query(uri, value); }
        template <typename T> inline bool set_fragment(T &uri, const std::string &value) { return traits<T>::set_fragment(uri, value); }
        template <typename T> inline bool set_authority(T &uri, const std::string &value) { return traits<T>::set_authority(uri, value); }
        template <typename T> inline bool set_path(T &uri, const std::string &value) { return traits<T>::set_path(uri, value); }
        template <typename T> inline bool set_username(T &url, const std::string &value) { return traits<T>::set_username(url, value); }
        template <typename T> inline bool set_password(T &url, const std::string &value) { return traits<T>::set_password(url, value); }
        template <typename T> inline bool set_host(T &url, const std::string &value) { return traits<T>::set_host(url, value); }
        template <typename T> inline bool set_port(T &url, const std::string &value) { return traits<T>::set_port(url, value); }
        template <typename T> inline bool set_nid(T &urn, const std::string &value) { return traits<T>::set_nid(urn, value); }
        template <typename T> inline bool set_nss(T &urn, const std::string &value) { return traits<T>::set_nss(urn, value); }
        template <typename T> inline bool equal(const T &uri1, const T &uri2) { return traits<T>::equal(uri1, uri2); }
        template <typename T> inline bool less(const T &uri1, const T &uri2) { return traits<T>::less(uri1, uri2); }
        
        namespace content
        {
            template <typename T> inline std::string get_hierarchy(const T &content) { return traits<T>::get_hierarchy(content); }
            template <typename T> inline std::string get_query(const T &content) { return traits<T>::get_query(content); }
            template <typename T> inline std::string get_fragment(const T &content) { return traits<T>::get_fragment(content); }
            template <typename T> inline bool set_hierarchy(T &content, const std::string &value) { return traits<T>::set_hierarchy(content, value); }
            template <typename T> inline bool set_query(T &content, const std::string &value) { return traits<T>::set_query(content, value); }
            template <typename T> inline bool set_fragment(T &content, const std::string &value) { return traits<T>::set_fragment(content, value); }
        }

        namespace hierarchy
        {
            template <typename T> inline std::string get_authority(const T &hierarchy) { return traits<T>::get_authority(hierarchy); }
            template <typename T> inline std::string get_path(const T &hierarchy) { return traits<T>::get_path(hierarchy); }
            template <typename T> inline bool set_authority(T &hierarchy, const std::string &value) { return traits<T>::set_authority(hierarchy, value); }
            template <typename T> inline bool set_path(T &hierarchy, const std::string &value) { return traits<T>::set_path(hierarchy, value); }
        }

        namespace url
        {
            template <typename T> inline std::string get_username(const T &authority) { return traits<T>::get_username(authority); }
            template <typename T> inline std::string get_password(const T &authority) { return traits<T>::get_password(authority); }
            template <typename T> inline std::string get_host(const T &authority) { return traits<T>::get_host(authority); }
            template <typename T> inline std::string get_port(const T &authority) { return traits<T>::get_port(authority); }
            template <typename T> inline bool set_username(T &authority, const std::string &value) { return traits<T>::set_username(authority, value); }
            template <typename T> inline bool set_password(T &authority, const std::string &value) { return traits<T>::set_password(authority, value); }
            template <typename T> inline bool set_host(T &authority, const std::string &value) { return traits<T>::set_host(authority, value); }
            template <typename T> inline bool set_port(T &authority, const std::string &value) { return traits<T>::set_port(authority, value); }
        }

        namespace urn
        {
            template <typename T> inline std::string get_nid(const T &content) { return traits<T>::get_nid(content); }
            template <typename T> inline std::string get_nss(const T &content) { return traits<T>::get_nss(content); }
            template <typename T> inline bool set_nid(T &content, const std::string &value) { return traits<T>::set_nid(content, value); }
            template <typename T> inline bool set_nss(T &content, const std::string &value) { return traits<T>::set_nss(content, value); }
        }

        //--------------------------------------------------------------------------------
        // utility function for testing implementations

        template <typename T>
        bool test(T &uri, std::ostream &stream)
        {
            set_uri(uri, "blank");
            stream << "uri: " << get_uri(uri) << std::endl;
            set_scheme(uri, "scheme");
            stream << "  set_scheme('scheme'): " << get_scheme(uri) << std::endl;
            stream << "uri: " << get_uri(uri) << std::endl;
            set_content(uri, "content");
            stream << "  set_content('content'): " << get_content(uri) << std::endl;
            stream << "uri: " << get_uri(uri) << std::endl;

            /*
            content::set_hierarchy(get_content(uri), "content_hierarchy");
            stream << "  content::set_hierarchy('content_hierarchy'): " << content::get_hierarchy(get_content(uri)) << std::endl;
            stream << "uri: " << get_uri(uri) << std::endl;
            content::set_query(get_content(uri), "content_query");
            stream << "  content::set_query('content_query'): " << content::get_query(get_content(uri)) << std::endl;
            stream << "uri: " << get_uri(uri) << std::endl;
            content::set_fragment(get_content(uri), "content_fragment");
            stream << "  content::set_fragment('content_fragment'): " << content::get_fragment(get_content(uri)) << std::endl;
            stream << "uri: " << get_uri(uri) << std::endl;
            hierarchy::set_authority(get_hierarchy(uri), "hierarchy_authority");
            stream << "  hierarchy::set_authority('hierarchy_authority'): " << hierarchy::get_authority(get_hierarchy(uri)) << std::endl;
            stream << "uri: " << get_uri(uri) << std::endl;
            hierarchy::set_path(get_hierarchy(uri), "hierarchy_path");
            stream << "  hierarchy::set_path('hierarchy_path'): " << hierarchy::get_path(get_hierarchy(uri)) << std::endl;
            stream << "uri: " << get_uri(uri) << std::endl;
            url::set_username(get_authority(uri), "url_username");
            stream << "  url::set_username('url_username'): " << url::get_username(get_authority(uri)) << std::endl;
            stream << "uri: " << get_uri(uri) << std::endl;
            url::set_password(get_authority(uri), "url_password");
            stream << "  url::set_password('url_password'): " << url::get_password(get_authority(uri)) << std::endl;
            stream << "uri: " << get_uri(uri) << std::endl;
            url::set_host(get_authority(uri), "url_host");
            stream << "  url::set_host('url_host'): " << url::get_host(get_authority(uri)) << std::endl;
            stream << "uri: " << get_uri(uri) << std::endl;
            url::set_host(get_authority(uri), "url_port");
            stream << "  url::set_port('url_port'): " << url::get_port(get_authority(uri)) << std::endl;
            stream << "uri: " << get_uri(uri) << std::endl;
            */

            set_hierarchy(uri, "hierarchy_authority/hierarchy_path");
            stream << "  set_hierarchy('hierarchy_authority/hierarchy_path'): " << get_hierarchy(uri) << std::endl;
            stream << "uri: " << get_uri(uri) << std::endl;
            set_query(uri, "query");
            stream << "  set_query('query'): " << get_query(uri) << std::endl;
            stream << "uri: " << get_uri(uri) << std::endl;
            set_fragment(uri, "fragment");
            stream << "  set_fragment('fragment'): " << get_fragment(uri) << std::endl;
            stream << "uri: " << get_uri(uri) << std::endl;
            set_authority(uri, "authority");
            stream << "  set_authority('authority'): " << get_authority(uri) << std::endl;
            stream << "uri: " << get_uri(uri) << std::endl;
            set_path(uri, "/path/to/something.ext");
            stream << "  set_path('/path/to/something.ext'): " << get_path(uri) << std::endl;
            stream << "uri: " << get_uri(uri) << std::endl;
            set_username(uri, "username");
            stream << "  set_username('username'): " << get_username(uri) << std::endl;
            stream << "uri: " << get_uri(uri) << std::endl;
            set_password(uri, "password");
            stream << "  set_password('password'): " << get_password(uri) << std::endl;
            stream << "uri: " << get_uri(uri) << std::endl;
            set_host(uri, "host");
            stream << "  set_host('host'): " << get_host(uri) << std::endl;
            stream << "uri: " << get_uri(uri) << std::endl;
            set_host(uri, "port");
            stream << "  set_port('port'): " << get_port(uri) << std::endl;
            stream << "uri: " << get_uri(uri) << std::endl;

            stream << std::endl;

            set_uri(uri, "urn:content");
            stream << "uri: " << get_uri(uri) << std::endl;
            urn::set_nid(get_content(uri), "urn_nid");
            stream << "  urn::set_nid('urn_nid'): " << urn::get_nid(get_content(uri)) << std::endl;
            stream << "uri: " << get_uri(uri) << std::endl;
            urn::set_nss(get_content(uri), "urn_nss");
            stream << "  urn::set_nss('urn_nss'): " << urn::get_nid(get_content(uri)) << std::endl;
            stream << "uri: " << get_uri(uri) << std::endl;

            stream << std::endl;

            if(!equal(uri, uri))
                stream << "uri != uri" << std::endl;
            if(less(uri, uri))
                stream << "uri < uri" << std::endl;

            return true;
        }

        //--------------------------------------------------------------------------------
        // traits implementation for std::string

        template <>
        struct traits<std::string>
        {
            static std::string get_uri(const std::string &uri)
            {
                return uri;
            }

            static std::string get_scheme(const std::string &uri)
            {
                std::string::size_type pos = uri.find(':');
                return (pos == uri.npos) ? std::string() : uri.substr(0, pos);
            }

            static std::string get_content(const std::string &uri)
            {
                std::string::size_type pos = uri.find(':');
                return (pos == uri.npos) ? uri : uri.substr(pos + 1);
            }

            static std::string get_hierarchy(const std::string &uri)
            {
                return content::get_hierarchy(get_content(uri));
            }

            static std::string get_query(const std::string &uri)
            {
                return content::get_query(get_content(uri));
            }

            static std::string get_fragment(const std::string &uri)
            {
                return content::get_fragment(get_content(uri));
            }

            static std::string get_authority(const std::string &uri)
            {
                return hierarchy::get_authority(get_hierarchy(uri));
            }

            static std::string get_path(const std::string &uri)
            {
                return hierarchy::get_path(get_hierarchy(uri));
            }

            static std::string get_username(const std::string &url)
            {
                return url::get_username(get_authority(url));
            }

            static std::string get_password(const std::string &url)
            {
                return url::get_password(get_authority(url));
            }

            static std::string get_host(const std::string &url)
            {
                return url::get_host(get_authority(url));
            }

            static std::string get_port(const std::string &url)
            {
                return url::get_port(get_authority(url));
            }

            static std::string get_nid(const std::string &urn)
            {
                return urn::get_nid(get_content(urn));
            }

            static std::string get_nss(const std::string &urn)
            {
                return urn::get_nss(get_content(urn));
            }

            static bool set_uri(std::string &uri, const std::string &value)
            {
                uri = value;
                return true;
            }

            static bool set_scheme(std::string &uri, const std::string &value)
            {
                if(value.empty())
                    uri = get_content(uri);
                else
                    uri = value + ":" + get_content(uri);
                return true;
            }

            static bool set_content(std::string &uri, const std::string &value)
            {
                std::string scheme = get_scheme(uri);
                if(scheme.empty())
                    uri = value;
                else
                    uri = scheme + ":" + value;
                return true;
            }

            static bool set_hierarchy(std::string &uri, const std::string &value)
            {
                std::string content = get_content(uri);
                content::set_hierarchy(content, value);
                set_content(uri, content);
                return true;
            }

            static bool set_query(std::string &uri, const std::string &value)
            {
                std::string content = get_content(uri);
                content::set_query(content, value);
                set_content(uri, content);
                return true;
            }

            static bool set_fragment(std::string &uri, const std::string &value)
            {
                std::string content = get_content(uri);
                content::set_fragment(content, value);
                set_content(uri, content);
                return true;
            }

            static bool set_authority(std::string &uri, const std::string &value)
            {
                std::string hierarchy = get_hierarchy(uri);
                hierarchy::set_authority(hierarchy, value);
                set_hierarchy(uri, hierarchy);
                return true;
            }

            static bool set_path(std::string &uri, const std::string &value)
            {
                std::string hierarchy = get_hierarchy(uri);
                hierarchy::set_path(hierarchy, value);
                set_hierarchy(uri, hierarchy);
                return true;
            }

            static bool set_username(std::string &url, const std::string &value)
            {
                std::string authority = get_authority(url);
                url::set_username(authority, value);
                set_authority(url, authority);
                return true;
            }

            static bool set_password(std::string &url, const std::string &value)
            {
                std::string authority = get_authority(url);
                url::set_password(authority, value);
                set_authority(url, authority);
                return true;
            }

            static bool set_host(std::string &url, const std::string &value)
            {
                std::string authority = get_authority(url);
                url::set_host(authority, value);
                set_authority(url, authority);
                return true;
            }

            static bool set_port(std::string &url, const std::string &value)
            {
                std::string authority = get_authority(url);
                url::set_port(authority, value);
                set_authority(url, authority);
                return true;
            }

            static bool set_nid(std::string &urn, const std::string &value)
            {
                std::string content = get_content(urn);
                urn::set_nid(content, value);
                set_content(urn, content);
                return true;
            }

            static bool set_nss(std::string &urn, const std::string &value)
            {
                std::string content = get_content(urn);
                urn::set_nss(content, value);
                set_content(urn, content);
                return true;
            }

            static bool equal(const std::string &uri1, const std::string &uri2)
            {
                return uri1 == uri2;
            }

            static bool less(const std::string &uri1, const std::string &uri2)
            {
                return uri1 < uri2;
            }
        };

        namespace content
        {
            template <>
            struct traits<std::string>
            {
                static std::string get_hierarchy(const std::string &content)
                {
                    std::string::size_type pos = content.find('?');
                    if(pos == content.npos)
                        pos = content.find('#');
                    return (pos == content.npos) ? content : content.substr(0, pos);
                }

                static std::string get_query(const std::string &content)
                {
                    std::string::size_type qpos = content.find('?');
                    if(qpos == content.npos)
                        return std::string();
                    std::string::size_type fpos = content.find('#');
                    return (fpos == content.npos) ? content.substr(qpos + 1) : content.substr(qpos + 1, fpos - qpos - 1);
                }

                static std::string get_fragment(const std::string &content)
                {
                    std::string::size_type fpos = content.find('#');
                    return (fpos == content.npos) ? std::string() : content.substr(fpos + 1);
                }

                static bool set_hierarchy(std::string &content, const std::string &value)
                {
                    std::string query = get_query(content);
                    std::string fragment = get_fragment(content);
                    content = value;
                    if(!query.empty())
                        content += "?" + query;
                    if(!fragment.empty())
                        content += "#" + fragment;
                    return true;
                }

                static bool set_query(std::string &content, const std::string &value)
                {
                    std::string hierarchy = get_hierarchy(content);
                    std::string fragment = get_fragment(content);
                    content = hierarchy;
                    if(!value.empty())
                        content += "?" + value;
                    if(!fragment.empty())
                        content += "#" + fragment;
                    return true;
                }

                static bool set_fragment(std::string &content, const std::string &value)
                {
                    std::string hierarchy = get_hierarchy(content);
                    std::string query = get_query(content);
                    content = hierarchy;
                    if(!query.empty())
                        content += "?" + query;
                    if(!value.empty())
                        content += "#" + value;
                    return true;
                }
            };
        }

        namespace hierarchy
        {
            template <>
            struct traits<std::string>
            {
                static std::string get_authority(const std::string &hierarchy)
                {
                    if(hierarchy.find("//") != 0)
                        return std::string();
                    std::string::size_type pos = hierarchy.find('/', 2);
                    return (pos == hierarchy.npos) ? hierarchy.substr(2) : hierarchy.substr(2, pos - 2);
                }

                static std::string get_path(const std::string &hierarchy)
                {
                    if(hierarchy.find("//") != 0)
                        return hierarchy;
                    std::string::size_type pos = hierarchy.find('/', 2);
                    return (pos == hierarchy.npos) ? std::string() : hierarchy.substr(pos);
                }

                static bool set_authority(std::string &hierarchy, const std::string &value)
                {
                    std::string path = get_path(hierarchy);
                    hierarchy = value.empty() ? path : "//" + value + path;
                    return true;
                }

                static bool set_path(std::string &hierarchy, const std::string &value)
                {
                    std::string authority = get_authority(hierarchy);
                    hierarchy = authority.empty() ? value : "//" + authority + value;
                    return true;
                }
            };
        }

        namespace url
        {
            template <>
            struct traits<std::string>
            {
                static std::string get_username(const std::string &authority)
                {
                    std::string result = authority;
                    std::string::size_type pos = result.find('@');
                    if(pos == result.npos)
                        return std::string();
                    result = result.substr(0, pos);
                    pos = result.find(':');
                    return (pos == result.npos) ? result : result.substr(0, pos);
                }

                static std::string get_password(const std::string &authority)
                {
                    std::string result = authority;
                    std::string::size_type pos = result.find('@');
                    if(pos == result.npos)
                        return std::string();
                    result = result.substr(0, pos);
                    pos = result.find(':');
                    return (pos == result.npos) ? std::string() : result.substr(pos + 1);
                }

                static std::string get_host(const std::string &authority)
                {
                    std::string result = authority;
                    std::string::size_type pos = result.find('@');
                    if(pos != result.npos)
                        result = result.substr(pos + 1);
                    pos = result.find(':');
                    return (pos == result.npos) ? result : result.substr(0, pos);
                }

                static std::string get_port(const std::string &authority)
                {
                    std::string result = authority;
                    std::string::size_type pos = result.find('@');
                    if(pos != result.npos)
                        result = result.substr(pos + 1);
                    pos = result.find(':');
                    return (pos == result.npos) ? std::string() : result.substr(pos + 1);
                }

                static bool set_username(std::string &authority, const std::string &value)
                {
                    std::string password = get_password(authority);
                    std::string host = get_host(authority);
                    std::string port = get_port(authority);
                    authority = value;
                    if(!password.empty())
                        authority += ":" + password;
                    if(!authority.empty())
                        authority += "@";
                    authority += host;
                    if(!port.empty())
                        authority += ":" + port;
                    return true;
                }

                static bool set_password(std::string &authority, const std::string &value)
                {
                    std::string username = get_username(authority);
                    std::string host = get_host(authority);
                    std::string port = get_port(authority);
                    authority = username;
                    if(!value.empty())
                        authority += ":" + value;
                    if(!authority.empty())
                        authority += "@";
                    authority += host;
                    if(!port.empty())
                        authority += ":" + port;
                    return true;
                }

                static bool set_host(std::string &authority, const std::string &value)
                {
                    std::string username = get_username(authority);
                    std::string password = get_password(authority);
                    std::string port = get_port(authority);
                    authority = username;
                    if(!password.empty())
                        authority += ":" + password;
                    if(!authority.empty())
                        authority += "@";
                    authority += value;
                    if(!port.empty())
                        authority += ":" + port;
                    return true;
                }

                static bool set_port(std::string &authority, const std::string &value)
                {
                    std::string username = get_username(authority);
                    std::string password = get_password(authority);
                    std::string host = get_host(authority);
                    authority = username;
                    if(!password.empty())
                        authority += ":" + password;
                    if(!authority.empty())
                        authority += "@";
                    authority += host;
                    if(!value.empty())
                        authority += ":" + value;
                    return true;
                }
            };
        }

        namespace urn
        {
            template <>
            struct traits<std::string>
            {
                static std::string get_nid(const std::string &content)
                {
                    std::string::size_type pos = content.find(':');
                    return (pos == content.npos) ? std::string() : content.substr(0, pos);
                }

                static std::string get_nss(const std::string &content)
                {
                    std::string::size_type pos = content.find(':');
                    return (pos == content.npos) ? content : content.substr(pos + 1);
                }

                static bool set_nid(std::string &content, const std::string &value)
                {
                    content = value + ":" + get_nss(content);
                    return true;
                }

                static bool set_nss(std::string &content, const std::string &value)
                {
                    content = get_nid(content) + ":" + value;
                    return true;
                }
            };
        }

    }
}